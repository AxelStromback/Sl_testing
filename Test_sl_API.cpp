#include <iostream>
#include <string>
#include <curl/curl.h>
#include <fstream>
#include <ctime>

using namespace std;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* output) {
    output->append((char*)contents, size * nmemb);
    return size * nmemb;
}

string getCurrentDigiTime() {
    time_t now = time(0);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return string(buffer);
}

int main() {
    cout << "Fetching train data..." << endl;
    
    CURL* curl;
    string response;
    
    string url = "https://realtime-api.trafiklab.se/v1/departures/740021662?key=395de3f58dee42a7b189a836efe75928";
    
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_CAINFO, "curl-ca-bundle.crt");
        
        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        
        if (res == 0) {
           size_t pos = response.find("\"realtime\":\"");
            string departureTime = response.substr(pos + 23, 5); // Gets "22:04"

            // Find designation (line number)
            pos = response.find("\"designation\":\"", pos);
            string line = response.substr(pos + 15, 2); // Gets "10"

            // Find direction
            pos = response.find("\"direction\":\"", pos);
            size_t endPos = response.find("\"", pos + 13);
            string direction = response.substr(pos + 13, endPos - pos - 13);
            cout << line <<  "   "  << departureTime  << endl;

            cout << getCurrentDigiTime() << "    "<<  endl;
            cout << departureTime << endl;
            

            // Save to fileB
            ofstream file("train_data.txt", ios::app);
            file << "\n=== " << getCurrentDigiTime() << " ===\n";
            file << response << "\n\n";
            file.close();
            
            cout << "Data saved to train_data.txt!" << endl;
        } else {
            cout << "Error fetching data!" << endl;
        }
    }
    
    return 0;
}