#include <curl/curl.h>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t totalSize = size * nmemb;
  string *response = static_cast<string *>(userp);
  response->append(static_cast<char *>(contents), totalSize);
  return totalSize;
}

string get_request(const string &url) {
  cout << "Entered request\n";

  CURL *curl = curl_easy_init();
  string result;

  cout << "Created vars\n";

  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    cout << "RUNNED:\t" << "curl_easy_setopt(curl, CURLOPT_URL, url.c_str());"
         << endl;

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
      cerr << "curl_easy_perform failed: " << curl_easy_strerror(res) << endl;
    } else {
      cout << "RUNNED:\t" << "curl_easy_perform(curl);" << endl;
    }

    curl_easy_cleanup(curl);
    cout << "RUNNED:\t" << "curl_easy_cleanup(curl);" << endl;
  }

  return result;
}

string get_filename_from_url(const string &url) {
  size_t end = url.find_last_not_of('/');
  if (end == string::npos) {
    return "output";
  }

  size_t start = url.find_last_of('/', end);
  if (start == string::npos) {
    return url.substr(0, end + 1);
  }

  return url.substr(start + 1, end - start);
}

void save_to_file(string title, string content) {
  // Create and open a text file
  ofstream MyFile(title + ".txt");

  // Write to the file
  MyFile << content;

  // Close the file
  MyFile.close();
}

int main() {
  curl_global_init(CURL_GLOBAL_ALL);

  cout << "Scraping data" << endl;
  string url = "https://www.scrapingcourse.com/ecommerce/";
  string html_document = get_request(url);
  string title = get_filename_from_url(url);

  cout << "Data scraped" << endl;

  cout << html_document;

  curl_global_cleanup();

  save_to_file(title, html_document);
  return 0;
}
