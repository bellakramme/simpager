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
#include <stdlib.h>
#include <time.h>

/**
 * NOTE: LRU output may differ due to changes in process clock time.
*/

/**
 * To do:
 * - make ./a.out < inputfile.txt with cin work
*/

void process_file(int argc, std::string fname, std::vector<int>& reference_string,
    int &num_frames, std::vector<std::string> &mneumonics);
void process_input(std::string fname, std::vector<int>& reference_string,
    int &num_frames, std::vector<std::string> &mneumonics);
void process_mneumonics(std::vector<std::string> mneumonics, 
    std::vector<int> &results, std::vector<int> reference_string, int num_frames);
bool in_page_table(int page, int pages[], int n);
int fifo(int num_frames, std::vector<int> reference_string);
int lru(int num_frames, std::vector<int> reference_string);
int opt(int num_frames, std::vector<int> reference_string);
int randalg(int num_frames, std::vector<int> reference_string);
void print_output(std::vector<int> reference_string, std::vector<int> results,
    int num_frames, std::vector<std::string> mneumonics);


int main(int argc, char * argv[])
{
    std::string filename = argv[1];     // contains the file name
    std::vector<int> reference_string;  // contains the reference list/string
    std::vector<std::string> mneumonics;    // contains all mneumonics (FIFO, LRU, etc)
    std::vector<int> results;   // contains the page faults of all mneumonics, corresponding to order of menumonics vector
    int num_frames = 0;     // number of frames

    process_file(argc, filename, reference_string, num_frames, mneumonics);
    // process_input(filename, reference_string, num_frames, mneumonics);
    results.resize(mneumonics.size());
    for(int i=0; i<mneumonics.size(); i++) {
        results[i] = -1;
    }

    // debug
    // std::cout << "Mneumonics: " << mneumonics.size();
    // std::cout << "Results: " << results.size();

    process_mneumonics(mneumonics, results, reference_string, num_frames);
    print_output(reference_string, results, num_frames, mneumonics);
}


/**
 * @brief Processes the text input file.
 * @param the number of arguments in the command line arguments.
 * @param fname the name of the input file.
 * @param reference_string the variable which will contain the numbers representing the reference string
 *      from the input file.
 * @param num_frames the variable which will contain the number of frames from the input file.
 * @param mneumonics the vector of strings that will contain all mneumonics from input file.
 * @post Variable reference_string contains string of numbers in input file as int values, num_frames
 *      contains number of frames/pages that the page table will use, and mneumonics contains all
 *      page replacement method mneumonics from input file. 
*/
void process_file(int argc, std::string fname, std::vector<int>& reference_string,
    int &num_frames, std::vector<std::string> &mneumonics)
{  
    // throw exception if no input file.
    if(argc < 2)
        throw std::invalid_argument("usage: ./simpager data.txt");

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
void process_input(std::string fname, std::vector<int>& reference_string,
    int &num_frames, std::vector<std::string> &mneumonics)
{
    std::string rstr;
    std::string line;

    std::getline(std::cin, rstr);
    for(int i=0; i<rstr.length(); i+=2) {
        reference_string.push_back(rstr[i] - '0');
    }

    std::cout << "\n" << rstr << "\n";

    std::getline(std::cin, line);
    num_frames = stoi(line);
    // throw exception if less than one frame.
    if(num_frames < 1) {
        throw std::invalid_argument("input file must include at least one page.");
    }

    std::cout << "\n" << num_frames << "\n";

    while(std::getline(std::cin, line))  //input from the file in.txt
    {
        mneumonics.push_back(line);
        std::cout << line << "\n";   //output to the file out.txt
    }
}


/**
 * @brief Iterates through mneumonics vector and processes all mneumonics using FIFO, 
 *      LRU, OPT, and RAND methods, then inserts their result (number of page faults)
 *      into the results vector.
 * @param mneumonics the vector of strings containing all mneumonics.
 * @param results the vector of ints which will contain the final page faults for all the
 *      mneumonics, corresponding to the positions of all mneumonics in the mneumonics
 *      string (i.e. FIFO at index 0 will contain its page faults at results[0]).
 * @param reference_string the vector of ints containing the reference string.
 * @param num_frames the number of frames/pages in the page table.
 * @post The vector results will contain the number of page faults by each mneumonic method.
*/
void process_mneumonics(std::vector<std::string> mneumonics, 
    std::vector<int> &results, std::vector<int> reference_string, int num_frames)
{
    int counter = 0;
    while(counter < results.size()) {
        if(mneumonics[counter] == "FIFO")
            results[counter] = fifo(num_frames, reference_string);
        else if(mneumonics[counter] == "LRU")
            results[counter] = lru(num_frames, reference_string);
        else if(mneumonics[counter] == "OPT")
            results[counter] = opt(num_frames, reference_string);
        else if(mneumonics[counter] == "RAND")
            results[counter] = randalg(num_frames, reference_string);
        else   
            std::cerr << "\nError: invalid input in file.\n";
        counter++;
    }
    // for(int counter = 0; counter < results.size(); counter++) {
    //     if(mneumonics[counter] == "FIFO")
    //         results[counter] = fifo(num_frames, reference_string);
    //     else if(mneumonics[counter] == "LRU")
    //         results[counter] = lru(num_frames, reference_string);
    //     else if(mneumonics[counter] == "OPT")
    //         // results[counter] = opt(num_frames, reference_string);
    //         std::cout << "Hi";
    //     else if(mneumonics[counter] == "RAND")
    //         results[counter] = randalg(num_frames, reference_string);
    //     else   
    //         std::cerr << "\nError: invalid input in file.\n";
    //     counter++;
    // }
}


/**
 * @brief Helper function to determine if the given page is in the page table.
 * @param page the page being searched for in the page table.
 * @param pages the page table.
 * @param n the size of the page table.
 * @return true if page is in pages; false if not.
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
 * @brief Runs the page table simulation using First-In, First-Out method.
 * @param num_frames the number of frames/pages in the page table.
 * @param reference_string the vector of ints containing the reference string.
 * @return the number of page faults caused by the FIFO method.
*/
int fifo(int num_frames, std::vector<int> reference_string)
{
    int page_faults = 0;
    int counter = 0;
    int pages[num_frames];

    // initializes all pages to -1 (invalid)
    for(int i=0; i<num_frames; i++) {
        pages[i] = -1;
    }

    for(int i=0; i<reference_string.size(); i++) {
        if(!in_page_table(reference_string[i], pages, num_frames)) {
            page_faults++;
            pages[counter] = reference_string[i];
            counter = (counter + 1) % num_frames;
        }
    }
    return page_faults;
}


/**
 * @brief Runs the page table simulation using Least Recently Used method.
 * @param num_frames the number of frames/pages in the page table.
 * @param reference_string the vector of ints containing the reference string.
 * @return the number of page faults caused by the LRU method.
*/
int lru(int num_frames, std::vector<int> reference_string)
{
    int page_faults = 0;
    int pages[num_frames];
    clock_t counters[num_frames];

    // initializes all pages to -1 (invalid)
    for(int i=0; i<num_frames; i++) {
        pages[i] = -1;
        counters[i] = 0;
    }

    for(int i=0; i<reference_string.size(); i++) {
        if(!in_page_table(reference_string[i], pages, num_frames)) {
            page_faults++;
            int victim = 0;

            // compare clock times to see max
            for(int i=1; i<num_frames; i++) {
                if(counters[i] < counters[victim])
                    victim = i;
            }

            // overwrite victim and set counters[victim] to current time
            pages[victim] = reference_string[i];
            counters[victim] = clock();
        }
        else {
            int index=-1;

            // get index of matching page
            for(int j=0; j<num_frames; j++) {
                if(pages[j] == reference_string[i]) {
                    index = j;
                }
            }

            // copy current clock time into counters at index of reference_string[i]
            counters[index] = clock();
        }

        // debug - uncomment to see page-by-page status of page table & counters
        // std::cout << "\n";
        // for(int i=0; i<num_frames; i++) {
        //     std::cout << pages[i] << "    ";
        // }
        // std::cout << "\n";
        // for(int i=0; i<num_frames; i++) {
        //     std::cout << counters[i] << " ";
        // }
        // std::cout << "\n";
    }
    return page_faults;
}


/**
 * @brief Runs the page table simulation using the Optimal method.
 * @param num_frames the number of frames/pages in the page table.
 * @param reference_string the vector of ints containing the reference string.
 * @return the number of page faults caused by the optimal method.
*/
int opt(int num_frames, std::vector<int> reference_string)
{
    int page_faults = 0;
    int pages[num_frames];
    int counters[num_frames];

    // initializes all pages to -1 (invalid)
    for(int i=0; i<num_frames; i++) {
        pages[i] = -1;
        counters[i] = 0;
    }

    for(int i=0; i<reference_string.size(); i++)  // go through all ref strings and check for page faults
    {
        if(!in_page_table(reference_string[i], pages, num_frames)) 
        {
            page_faults++;
            
            // replace page that is furthest away
            for(int k = 0; k < num_frames; k++) {   // for all pages in page table; corresponds with counters[]
                for(int j=i+1; j<reference_string.size(); j++)      // go through entire ref string and check for k page
                {
                    if(pages[k] != reference_string[j])
                        counters[k]++;
                }
            }
            int max_index=0;
            for(int k=1; k<num_frames; k++) {
                if(counters[max_index] < counters[k])
                    max_index = k;
            }
            pages[max_index] = reference_string[i];
        }
    }
    return page_faults;
}


/**
 * @brief Runs the page table simulation using the Random method.
 * @param num_frames the number of frames/pages in the page table.
 * @param reference_string the vector of ints containing the reference string.
 * @return the number of page faults caused by the random method.
*/
int randalg(int num_frames, std::vector<int> reference_string)
{
    srand(time(NULL));
    int page_faults = 0;
    int pages[num_frames];

    // initializes all pages to -1 (invalid)
    for(int i=0; i<num_frames; i++) {
        pages[i] = -1;
    }

    for(int i=0; i<reference_string.size(); i++) {
        if(!in_page_table(reference_string[i], pages, num_frames)) {
            page_faults++;
            int index = rand() & num_frames;
            pages[index] = reference_string[i];
        }
    }
    return page_faults;
}


/**
 * @brief Prints the results of the simulation.
 * @param reference_string the vector of ints containing the reference string.
 * @param results the vector of ints containing the results of the simulation corresponding to
 *      mneumonics; i.e., if mneumonics[i] == FIFO and results[i] == 15, then FIFO has 15 page faults
 * @param num_frames the number of frames/pages in the page table.
 * @param mneumonics the vector of strings containing the mneumonics included in the input file.
 * @post Text is output to the stream.
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
    std::cout << "\n\n";
}