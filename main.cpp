#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

std::mutex listMutex;

struct Entry {
	int number;
	std::string text;
	struct Entry* next;
	struct Entry* prev;
};


struct Entry* list = nullptr;
static unsigned int numElems = 0;

void listAddElement (int number, std::string text)
{
	listMutex.lock();
	if (list == nullptr) {
		list = new Entry;
		list->prev = nullptr;
		list->next = nullptr;
		list->text = text;
		list->number = number;
	} else {
		Entry* curElem = list;
		Entry* newElem = new Entry;
		while (curElem->next != nullptr) {
			curElem = curElem->next;
		}
		curElem->next = newElem;
		newElem->next = nullptr;
		newElem->prev = curElem;
		newElem->number = number;
		newElem->text = text;
	}
	numElems++;
	listMutex.unlock();
}

void printList()
{
	Entry* cur = list;
	
	while (cur->next != nullptr) {
		std::cout << cur->number << " : " << cur->text << std::endl;
		cur = cur->next;
	}
}

void listSort()
{
	int tempNumber;
	std::string tempText;
	if (list != nullptr) {
		Entry* cur = list;
		for (Entry* i = list; i->next != nullptr; i = i->next) {
			for (Entry* j = list; j->next != nullptr; j = j->next) {
				if (i->number < j->number) {
					tempNumber = i->number;
					tempText = i->text;
					i->number = j->number;
					i->text = j->text;
					j->number = tempNumber;
					j->text = tempText;
				}
			}
		}
	} else {
		std::cout << "ERROR! Called Sort on non existing list" << std::endl;
	}
}

void listSortByText()
{
	int tempNumber;
	std::string tempText;
	if (list != nullptr) {
		Entry* cur = list;
		for (Entry* i = list; i->next != nullptr; i = i->next) {
			for (Entry* j = list; j->next != nullptr; j = j->next) {
				if (i->text.compare(j->text) > 0 ) {
					tempNumber = i->number;
					tempText = i->text;
					i->number = j->number;
					i->text = j->text;
					j->number = tempNumber;
					j->text = tempText;
				}
			}
		}
	} else {
		std::cout << "ERROR! Called Sort on non existing list" << std::endl;
	}
}


void listDelete()
{
	if (list != nullptr) {
		Entry* cur = list;
		Entry* next;
		while (cur->next != nullptr) {
			next = cur->next;
			delete cur;
			cur = cur->next;
		}
	}
}

void DataGeneratorThread()
{
	static const char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	
	std::cout << "Created DataGeneratorThread" << std::endl;
	
	srand( (unsigned) time(NULL) * getpid());
	
	while(numElems < 100) {
		std::string newText = "";
		unsigned int textLen = rand() % 10 +1; //max len = 10
		int newNumber = rand();
		
		for (int i = 0; i < textLen; i++) {
			newText += chars[rand() % sizeof(chars) -1];
		}
		std::cout << "New Item Number: " << newNumber << " Text: " << newText << std::endl;
		listAddElement(newNumber, newText);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	std::cout<<"Exit DataGeneratorThread" << std::endl;
}

void DataSorterThread()
{
	unsigned int sortedElems = 0;
	std::cout<<"Created DataSorterThread" << std::endl;		
	
	while(true) {
		listMutex.lock();
		if (numElems - sortedElems >= 10) {
			std::cout << "Sorting " << numElems << " Elements" << std::endl;
			listSort();
			sortedElems = numElems;
		}
		if(numElems >= 100) {
		 	listMutex.unlock();
		 	break;
		}
		listMutex.unlock();
	
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}

	std::cout<<"Exit DataSorterThread" << std::endl;
}

int main()
{
    std::thread dataGen(DataGeneratorThread);
    std::thread dataSort(DataSorterThread);
    dataGen.join();
    dataSort.join();
    std::cout << "List sorted by number: " << std::endl;
    listSort();
    printList();
    std::cout << "List sorted by text: " << std::endl;
    listSortByText();
    printList();

	system("pause");
	listDelete();

    return 0;
}
