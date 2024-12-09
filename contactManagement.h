// contactManagement.h
// Header file for Contact Management System

#ifndef CONTACTMANAGEMENT_H
#define CONTACTMANAGEMENT_H

// Maximum limits for contact list and field lengths
#define MAX_CONTACTS 100    // Maximum number of contacts
#define MAX_NAME_LEN 30     // Maximum length of contact name
#define MAX_ADDRESS_LEN 50  // Maximum length of contact address
#define MAX_EMAIL_LEN 30    // Maximum length of contact email
#define MAX_PHONE_LEN 20    // Maximum length of phone number

// Structure for storing contact details
struct Contact {
    char phone[MAX_PHONE_LEN];      // Phone number 
    char name[MAX_NAME_LEN];        // Contact name
    char address[MAX_ADDRESS_LEN];  // Contact address
    char email[MAX_EMAIL_LEN];      // Contact email
};

// Function declarations 
void addContact(struct Contact contacts[], int *count);           // Add a contact
void listContacts(struct Contact contacts[], int count);          // Display all contacts
void searchContact(struct Contact contacts[], int count);         // Find a contact by name
void editContact(struct Contact contacts[], int count);           // Update contact details
void deleteContact(struct Contact contacts[], int *count);        // Remove a contact
void saveContactsToFile(struct Contact contacts[], int count);    // Save to file
void loadContactsFromFile(struct Contact contacts[], int *count); // Load from file
void displayMenu();                                               // Show the menu

#endif // End of contactManegement header


