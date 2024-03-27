/**
 * Bella Kramme
 * Operating Systems
 * Programming Project #6: Simulation of Page Replacement Strategies
 * April 3, 2024
 * Instructor: Dr. Siming Liu
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>

/**
 * To do:
 * - fix LRU
 * - OPT
 * - RAND
 * - documentation
*/

void process_file(std::string fname, std::vector<int>& reference_string,
    int &num_frames, std::vector<std::string> &mneumonics);
void process_mneumonics(std::vector<std::string> mneumonics, 
    std::vector<int> &results, std::vector<int> reference_string, int num_frames);
bool in_page_table(int page, int pages[], int n);
int fifo(int num_frames, std::vector<int> reference_string);
int lru(int num_frames, std::vector<int> reference_string);
void print_output(std::vector<int> reference_string, std::vector<int> results,
    int num_frames, std::vector<std::string> mneumonics);


int main(int argc, char * argv[])
{
    // throw exception if no input file.
    if(argc < 2)
        throw std::invalid_argument("usage: ./simpager data.txt");

    std::string filename = argv[1];     // contains the file name
    std::vector<int> reference_string;  // contains the reference list/string
    std::vector<std::string> mneumonics;    // contains all mneumonics (FIFO, LRU, etc)
    // std::vector<int> pages;     // contains all pages
    std::vector<int> results;   // contains the results of all mneumonics, corresponding to order of menumonics vector
    int num_frames = 0;     // number of frames

    process_file(filename, reference_string, num_frames, mneumonics);
    // pages.resize(num_frames);
    results.resize(num_frames);

    process_mneumonics(mneumonics, results, reference_string, num_frames);
    print_output(reference_string, results, num_frames, mneumonics);
}


/**
 * @brief Processes the text input file.
 * @param fname
 * @param reference_string
 * @param num_frames
 * @param mneumonics
 * @post 
*/
void process_file(std::string fname, std::vector<int>& reference_string,
    int &num_frames, std::vector<std::string> &mneumonics)
{  
    std::ifstream fin;
    fin.open(fname);

    // throw error if file does not exist / fails to open
    if(!fin)
        throw std::runtime_error("failed to open file.");

    std::string rstr;
    std::string line;

    std::getline(fin, rstr);
    // std::cout << refstr.length();
    for(int i=0; i<rstr.length(); i+=2) {
        // std::cout << refstr[i];
        // int num = refstr[i] - '0';
        // std::cout << num;
        reference_string.push_back(rstr[i] - '0');
    }

    std::getline(fin, line);
    num_frames = stoi(line);
    // throw exception if less than one frame.
    if(num_frames < 1) {
        throw std::invalid_argument("input file must include at least one page.");
    }

    while(!fin.eof()) {
        std::getline(fin, line);
        mneumonics.push_back(line);
    }

    fin.close();
}


/**
 * 
*/
void process_mneumonics(std::vector<std::string> mneumonics, 
    std::vector<int> &results, std::vector<int> reference_string, int num_frames)
{
    int counter = 0;
    while(counter < mneumonics.size()) {
        if(mneumonics[counter] == "FIFO")
            results[counter] = fifo(num_frames, reference_string);
        else if(mneumonics[counter] == "LRU")
            results[counter] = lru(num_frames, reference_string);
        else if(mneumonics[counter] == "OPT")
            // results[counter] = opt(num_frames, reference_string);
            break;
        else if(mneumonics[counter] == "RAND")
            // results[counter] = randalg(num_frames, reference_string);
            break;
        else   
            std::cerr << "\nError: invalid input in file.\n";
        counter++;
    }
}


/**
 * @brief Helper function to determine if the given page is in the page table.
 * @param page
 * @param pages
 * @param n
 * @return 
*/
bool in_page_table(int page, int pages[], int n)
{
    for(int i=0; i<n; i++) {
        if(pages[i] == page)
            return true;
    }
    return false;
}


/**
 * 
*/
int fifo(int num_frames, std::vector<int> reference_string)
{
    int page_faults = 0;
    int counter = 0;
    int rfsize = reference_string.size();
    int pages[num_frames];

    // initializes all pages to -1 (invalid)
    for(int i=0; i<num_frames; i++) {
        pages[i] = -1;
    }

    for(int i=0; i<rfsize; i++) {
        if(!in_page_table(reference_string[i], pages, num_frames)) {
            page_faults++;
            pages[counter] = reference_string[i];
            counter = (counter + 1) % num_frames;
        }
    }
    return page_faults;
}


/**
 * 
*/
int lru(int num_frames, std::vector<int> reference_string)
{
    int page_faults = 0;
    int rfsize = reference_string.size();
    int pages[num_frames];
    clock_t counters[num_frames];

    // initializes all pages to -1 (invalid)
    for(int i=0; i<num_frames; i++) {
        pages[i] = -1;
    }

    for(int i=0; i<rfsize; i++) {
        if(!in_page_table(reference_string[i], pages, num_frames)) {
            page_faults++;
            int victim = 0;

            // compare clock times to see max
            for(int i=1; i<num_frames; i++) {
                if(counters[i] > counters[victim])
                    victim = i;
            }

            // overwrite victim and set counters[victim] to current time
            pages[victim] = reference_string[i];
            counters[victim] = clock();
        }
        else {
            int index=-1;

            // get index of matching page
            for(int i=0; i<num_frames; i++) {
                if(pages[i] == reference_string[i]) {
                    index = i;
                    break;
                }
            }

            // copy current clock time into counters at index of reference_string[i]
            counters[index] = clock();
        }
    }
    return page_faults;
}


/**
 * @brief Prints the results of the simulation.
 * @param reference_string the vector of ints containing the list of references.
 * @param results the vector of ints containing the results of the simulation corresponding to
 *      mneumonics; i.e., if mneumonics[i] == FIFO and results[i] == 15, then FIFO has 15 page faults
 * @param num_frames the number of frames available.
 * @param mneumonics the vector of strings containing the mneumonics included in the input file.
 * @post WEFIOSJDJOSIJEF <-------
*/
void print_output(std::vector<int> reference_string, std::vector<int> results,
    int num_frames, std::vector<std::string> mneumonics)
{
    std::cout << "\nPage Reference String:\n";
    for(int i=0; i<reference_string.size(); i++) {
        std::cout << reference_string[i] << ' ';
    }
    std::cout <<"\nNumber of Frames: " << num_frames;
    for(int i=0; i<mneumonics.size(); i++) {
        std::cout << "\n" << mneumonics[i] << ": " << results[i];
    }
    std::cout << "\n";
}