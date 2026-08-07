#include "slottedpage.h"
#include <iostream>

int main() {
    SlottedPage my_page;

    // Data insert karo
    my_page.InsertRecord("Rahul - Age 20", 14);
    my_page.InsertRecord("Tarun - Age 21", 14);

    std::cout << "Total records in page: " << my_page.GetRecordCount() << std::endl;

    // Data read karo (Bina poora page search kiye!)
    char buffer[50];
    my_page.GetRecord(1, buffer); // Slot 1 yani Tarun

    std::cout << "Record at Slot 1: " << buffer << std::endl;

    return 0;
}