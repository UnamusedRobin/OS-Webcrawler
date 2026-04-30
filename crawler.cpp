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
  // Initialize libcurl global resources (must be called once before using curl)
  curl_global_init(CURL_GLOBAL_ALL);

  cout << "Entered request\n";

  // Initialize CURL handle
  CURL *curl = curl_easy_init();
  string result;


  if (curl) {
    // Set the URL to request
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

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
      cout << "Request successful, response stored in result string." << endl;
    }

    // Clean up CURL handle to free resources
    curl_easy_cleanup(curl);
    cout << "Clearing up any leftover resources" << endl;
  }

  // Clean up libcurl global resources (must be called after all curl operations)
  curl_global_cleanup();

  // Return the response body (empty string if CURL failed to initialize)
  return result;
}


// Main function - entry point of the program
int main() {
    string url = "http://example.com";  // URL to request
    
    get_request(url);
}
