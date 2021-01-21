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

void listSort(bool sortByText)
{
	Entry* temp;
	std::string tempText;
	if (list != nullptr && list->next != nullptr) {
		for (Entry* i = list; i->next != nullptr; i = i->next) {
			for (Entry* j = i->next; j->next != nullptr; j = j->next) {
				if ((j->prev->number < j->number && !sortByText) ||
				    (j->prev->text.compare(j->text) > 0 && sortByText)) {
				    Entry * l = j->prev;
					Entry * r = j;
					temp = l->prev;
					if(l->prev != nullptr) {
						l->prev->next = r;
					} else {
						list = r;
					}
					if(r->next != nullptr) {
						r->next->prev = l;
					}
					l->next = r->next;
					l->prev = r;
					r->prev = temp;
					r->next = l;
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
			cur = next;
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
			listSort(false);
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
    listSort(false);
    printList();
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "List sorted by text: " << std::endl;
    listSort(true);
    printList();

	system("pause");
	listDelete();

    return 0;
}
