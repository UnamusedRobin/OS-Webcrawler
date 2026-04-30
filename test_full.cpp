#include <curl/curl.h>
#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

// CURL METHODS
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t totalSize = size * nmemb;
    string *response = static_cast<string *>(userp);
    response->append(static_cast<char *>(contents), totalSize);
    return totalSize;
}

// Sends a GET request to the given URL and returns the response body as a string
string get_request(const string &url) {
    // Initialize a CURL session
    CURL *curl = curl_easy_init();
    string result;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);

        // IMPORTANT fixes
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");
        curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");

        // Perform the HTTP request
        CURLcode res = curl_easy_perform(curl);

        // Check if the request failed
        if (res != CURLE_OK) {
            cerr << "Failed: " << url << endl;
        }

        // Clean up and free CURL resources
        curl_easy_cleanup(curl);
    }

    return result;
}

struct Page {
    string url;
    string html;
};

// Shared structures
queue<string> url_queue;
queue<Page> page_queue;

unordered_set<string> visited;
unordered_map<string, vector<string>> inverted_index;

// Sync
mutex url_mutex, page_mutex, index_mutex, visited_mutex;
condition_variable page_cv;

bool done = false;

// Find every https links
vector<string> extract_links(const string &html) {
    vector<string> links;
    regex link_regex(R"(href\s*=\s*["']([^"']*)["'])", regex::icase);

    auto begin = sregex_iterator(html.begin(), html.end(), link_regex);
    auto end = sregex_iterator();

    for (auto i = begin; i != end; ++i) {
        string link = (*i)[1].str();

        // Only keep absolute URLs (simple filter)
        if (link.rfind("http", 0) == 0) {
            links.push_back(link);
        }
    }

    return links;
}

// Grabs the htmls
void downloader_worker() {
    while (true) {
        string url;

        {
            // Lock the URL queue to safely access shared data
            unique_lock<mutex> lock(url_mutex);

            // If no URLs are left, exit the worker thread
            if (url_queue.empty()) {
                return;
            }

            // Get the top url and remove it
            url = url_queue.front();
            url_queue.pop();
        }

        {
            // Lock the visited set to avoid duplicate downloads
            lock_guard<mutex> lock(visited_mutex);

           // If the URL was already downloaded, skip it
            if (visited.count(url)) {
                continue;
            }
            visited.insert(url);
        }

        cout << "[Downloading] " << url << endl;

        string html = get_request(url);

        {
            // Lock the page queue to safely add the downloaded result
            lock_guard<mutex> lock(page_mutex);
            cout << url << endl;

            page_queue.push({url, html});
        }

        // Tell another thread there is new page is available
        page_cv.notify_one();
    }
}

// Extracts a name from a URL by taking the last path component
// Parameter: url - the URL to extract name from
// Returns: the extracted name or "output" as default
string get_name_from_url(const string &url) {
  // Find the last forward slash to get the path component
  size_t end = url.find_last_not_of('/');
  
  // Find the second-to-last slash to isolate the name
  size_t start = url.find_last_of('/', end);
  if (start == string::npos) {
    // No second slash, entire URL is the name
    return url.substr(0, end + 1);
  }

  // Extract name between the slashes
  return url.substr(start + 1, end - start);
}

// Put item into index
void indexer_worker() {
    while (true) {
        Page page; // Will hold the next page to process

        {
            // Lock page queue and wait until there's work or we're done
            unique_lock<mutex> lock(page_mutex);

            // Wait until either:
            // - page_queue has items, OR
            // - done flag is set (no more pages will arrive)
            page_cv.wait(lock, [] {
                return !page_queue.empty() || done;
            });

            // There is no work left
            if (done && page_queue.empty()) {
                return;
            }

            // Get the next page
            page = page_queue.front();
            page_queue.pop();
        }

        cout << "[Indexing] " << page.url << endl;

        
        // find href="/title/<Title>" tag from html
        regex link_regex(R"delim(href="/title/([^"]+)")delim");


        auto words_begin = sregex_iterator(page.html.begin(), page.html.end(), link_regex);
        auto words_end = sregex_iterator();

        {
            
            lock_guard<mutex> lock(index_mutex);
            
            // Iterate over all matched tags in the HTML
            for (auto i = words_begin; i != words_end; ++i) {
                // Extract the word after /title/
                string word = (*i)[1].str();

                // Build full URL
                string full_url = "https://wiki.archlinux.org/title/" + word;


                // Add the URL to the list of pages containing this word
                inverted_index[word].push_back(full_url);
            }
        }

        // Extract https links from the page
        vector<string> links = extract_links(page.html);

        {
            // Lock URL queue to safely add new links for downloading
            lock_guard<mutex> lock(url_mutex);


            for (const auto &link : links) {
                url_queue.push(link);
            }
        }

    }
}

// Search the index and ask user in while loop
void search_loop() {
    string query;

    while (true) {
        cout << "\nSearch> ";
        cin >> query;

        lock_guard<mutex> lock(index_mutex);

        // Check if the query exists in the inverted index
        if (inverted_index.count(query)) {
            // If found, get all URLs with the query
            for (const auto &url : inverted_index[query]) {
                cout << url << endl;
            }
        } else {
            cout << "No results.\n";
        }
    }
}

int main() {
    curl_global_init(CURL_GLOBAL_ALL);

    string seed;
    cout << "Enter start URL: ";
    cin >> seed;

    // Root search node
    url_queue.push(seed);

    // Start threads
    vector<thread> downloaders;
    for (int i = 0; i < 4; ++i) {
        downloaders.emplace_back(downloader_worker);
    }

    thread indexer(indexer_worker);

    // Wait for downloaders to be done to install all html
    for (auto &t : downloaders) {
        t.join();
    }

    // Signal indexer to finish
    {
        lock_guard<mutex> lock(page_mutex);
        done = true;
    }
    page_cv.notify_all();

    indexer.join();

    cout << "\nCrawling + indexing done.\n";

    // Start search
    search_loop();

    curl_global_cleanup();
    return 0;
}