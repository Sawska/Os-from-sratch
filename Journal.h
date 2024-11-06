#ifndef JOURNAL_H
#define JOURNAL_H

#include <cstdio>
#include <cstdint>
#include <cstring>
#include "Filesystem.h"
#define BLOCK_SIZE 512    

const uint8_t OPERATION_START = 0;
const uint8_t OPERATION_WRITE = 1;
const uint8_t OPERATION_COMMIT = 2;

struct JournalEntry {
    uint32_t transactionID;
    uint8_t operationType; 
    uint32_t blockNumber;  
    uint32_t fileID;       
    uint8_t data[BLOCK_SIZE];
};


#include <fstream>
#include <vector>
#include <cstring>

const char* JOURNAL_FILE = "journal.bin";
int journalSize = 0;  
std::vector<JournalEntry> journalEntries;

void writeJournalEntry(const JournalEntry& entry) {
    std::ofstream outFile(JOURNAL_FILE, std::ios::binary | std::ios::app);
    if (outFile) {
        outFile.write(reinterpret_cast<const char*>(&entry), sizeof(JournalEntry));
        journalEntries.push_back(entry); 
        journalSize++;
        outFile.close();
    }
}

JournalEntry readJournalEntry(int index) {
    JournalEntry entry;
    if (index < 0 || index >= journalSize) {
        throw std::out_of_range("Invalid journal entry index");
    }

    std::ifstream inFile(JOURNAL_FILE, std::ios::binary);
    if (inFile) {
        inFile.seekg(index * sizeof(JournalEntry), std::ios::beg);
        inFile.read(reinterpret_cast<char*>(&entry), sizeof(JournalEntry));
        inFile.close();
    } else {
        throw std::runtime_error("Could not open journal file for reading");
    }
    return entry;
}

void applyJournalEntry(JournalEntry& entry) {
    if (entry.operationType == OPERATION_WRITE) {
        writeBlock(entry.blockNumber, entry.data); 
    }
}

void clearJournal() {
    std::ofstream outFile(JOURNAL_FILE, std::ios::trunc | std::ios::binary);
    if (outFile) {
        outFile.close();
    }
    journalEntries.clear(); 
    journalSize = 0;
}


extern int journalSize;               
extern uint32_t currentTransactionID; 
extern bool transactionInProgress;    

void startTransaction(uint32_t transactionID) {
    JournalEntry startEntry = {transactionID, OPERATION_START, 0, 0, {}};
    writeJournalEntry(startEntry);
}

void logWriteOperation(uint32_t transactionID, uint32_t blockNumber, const uint8_t* data) {
    JournalEntry entry = {transactionID, OPERATION_WRITE, blockNumber, 0, {}};
    memcpy(entry.data, data, BLOCK_SIZE);
    writeJournalEntry(entry);
}

void commitTransaction(uint32_t transactionID) {
    JournalEntry commitEntry = {transactionID, OPERATION_COMMIT, 0, 0, {}};
    writeJournalEntry(commitEntry);
}

void recoverFromJournal() {
    for (int i = 0; i < journalSize; i++) {
        JournalEntry entry = readJournalEntry(i);

        if (entry.operationType == OPERATION_START) {
            currentTransactionID = entry.transactionID;
            transactionInProgress = true;
        } else if (entry.operationType == OPERATION_WRITE && transactionInProgress) {
            applyJournalEntry(entry); 
        } else if (entry.operationType == OPERATION_COMMIT) {
            transactionInProgress = false;
        }
    }

    clearJournal();
}

#endif // JOURNAL_H
