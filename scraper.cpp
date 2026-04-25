// Standard library includes
#include <curl/curl.h>  // libcurl for HTTP requests
#include <fstream>      // File stream operations
#include <iostream>     // Input/output streams
#include <string>       // String class

using namespace std;



// Callback function for curl to write received data
// Parameters:
//   - contents: pointer to the data received from the server
//   - size: size of each data element
//   - nmemb: number of data elements
//   - userp: pointer to user data (our string buffer)
// Returns: total number of bytes processed
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
  // Calculate total size of incoming data
  size_t totalSize = size * nmemb;
  
  // Cast userp to our string pointer to store the response
  string *response = static_cast<string *>(userp);
  
  // Append the received data to our response string
  response->append(static_cast<char *>(contents), totalSize);
  
  // Return the number of bytes processed
  return totalSize;
}

// Performs an HTTP GET request to the specified URL
// Parameter: url - the URL to request
// Returns: the response body as a string
string get_request(const string &url) {
  cout << "Entered request\n";

  // Initialize CURL handle
  CURL *curl = curl_easy_init();
  string result;

  cout << "Created vars\n";

  if (curl) {
    // Set the URL to request
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    cout << "RUNNED:\t" << "curl_easy_setopt(curl, CURLOPT_URL, url.c_str());"
         << endl;

    // Set custom write callback function to handle response data
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    // Pass our result string as user data to the callback
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);

    // Perform the HTTP request
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
      // Print error message if request failed
      cerr << "curl_easy_perform failed: " << curl_easy_strerror(res) << endl;
    } else {
      cout << "RUNNED:\t" << "curl_easy_perform(curl);" << endl;
    }

    // Clean up CURL handle to free resources
    curl_easy_cleanup(curl);
    cout << "RUNNED:\t" << "curl_easy_cleanup(curl);" << endl;
  }

  // Return the response body (empty string if CURL failed to initialize)
  return result;
}

// Extracts a filename from a URL by taking the last path component
// Parameter: url - the URL to extract filename from
// Returns: the extracted filename or "output" as default
string get_filename_from_url(const string &url) {
  // Find the last forward slash to get the path component
  size_t end = url.find_last_not_of('/');
  
  // If no slash found, return default filename
  if (end == string::npos) {
    return "output";
  }
  
  // Find the second-to-last slash to isolate the filename
  size_t start = url.find_last_of('/', end);
  if (start == string::npos) {
    // No second slash, entire URL is the filename
    return url.substr(0, end + 1);
  }

  // Extract filename between the slashes
  return url.substr(start + 1, end - start);
}

// Saves content to a file at the specified path
// Parameters:
//   - filepath: directory path where the file will be saved
//   - filename: name of the file (without extension)
//   - content: the text content to write to the file
void save_file_to(string filepath, string filename, string content) {
  
  // Create and open a text file with .txt extension
  ofstream MyFile(filepath + "/" + filename + ".txt");

  // Write the content to the file
  MyFile << content;

  // Close the file to flush buffers and release resources
  MyFile.close();
}

// Main function - entry point of the program
int main() {
  // Initialize libcurl global resources (must be called once before using curl)
  curl_global_init(CURL_GLOBAL_ALL);
  
  // Define the directory path where files will be saved
  string filepath = "./data";

  // Print status message to indicate scraping is starting
  cout << "Scraping data" << endl;

  // Prompt message for user input
  string input = "Type in a url: "; // This is the url we are using that we are starting at
  string url;
  
  // Display prompt and read URL from user input
  cout << input;
  cin >> url;

  // Echo the entered URL back to the user
  cout << url << endl;
  
  // Perform HTTP GET request to fetch the webpage content
  string html_document = get_request(url);
  
  // Extract filename from URL to use as output filename
  string title = get_filename_from_url(url);

  // Print completion status
  cout << "Data scraped" << endl;

  // Clean up libcurl global resources (must be called after all curl operations)
  curl_global_cleanup();

  cout << html_document << endl;

  // Save the scraped content to a file
  // Parameters: filepath (directory), title (filename), html_document (content)
  save_file_to(filepath, title, html_document);
  

  // Exit successfully
  return 0;
}
