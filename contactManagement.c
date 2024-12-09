#include <stdio.h>  // For standard input/output functions
#include <stdlib.h>  // For general utilities such as memory allocation 
#include <string.h>  // For string manipulation functions
#include <ctype.h>   // For handling character-related functions like checking or converting case
#include "contactManagement.h"  // For Contact structure and related functions

// Define maximum sizes for different fields and the total number of contacts
#define MAX_CONTACTS 100
#define MAX_NAME_LEN 30
#define MAX_ADDRESS_LEN 50
#define MAX_EMAIL_LEN 30
#define MAX_PHONE_LEN 20

// Function declarations; these pretty much tell the compiler about functions defined later on in this code
void addContact(struct Contact contacts[], int *count);
void listContacts(struct Contact contacts[], int count);
void searchContact(struct Contact contacts[], int count);
void editContact(struct Contact contacts[], int count);
void deleteContact(struct Contact contacts[], int *count);
void saveContactsToFile(struct Contact contacts[], int count);
void loadContactsFromFile(struct Contact contacts[], int *count);
void displayMenu();
int compareContacts(const void *a, const void *b);

// substring search that is case insensitive and useful for our searchContact() function
char *caseInsensitiveStrStr(const char *haystack, const char *needle) {
    // If the needle (substring we're looking for) is empty, then the entire haystack is a match
    if (!*needle) return (char *)haystack;

    size_t hay_len = strlen(haystack); // Get the length of the haystack string
    size_t nee_len = strlen(needle); // Get the length of the needle string

    // Allocate memory for lowercase versions of haystack and needle
    // The + 1 is for the null terminator at the end of strings
    char *haystack_lower = malloc(hay_len + 1); 
    char *needle_lower = malloc(nee_len + 1);

    // Check if memory allocation was successful and if not, then clean up and exit
    if (!haystack_lower || !needle_lower) {
        if (haystack_lower) free(haystack_lower); // Free memory if allocated
        if (needle_lower) free(needle_lower); // Free memory if allocated
        return NULL; // Return "NULL" indicating failure
    }

    // convert haystack to lowercase so we can do case-insensitive comparison
    for (size_t i = 0; i <= hay_len; i++)
        haystack_lower[i] = (char)tolower((unsigned char)haystack[i]);

    // Convert needle to lowercase for the same reason
    for (size_t i = 0; i <= nee_len; i++)
        needle_lower[i] = (char)tolower((unsigned char)needle[i]);
    
    // use strstr To find the substring in lowercase
    char *pos = strstr(haystack_lower, needle_lower);
    char *result = NULL; // This will hold the location of the match in the original haystack string
    // if pos is not "NULL", it means a match was found
    if (pos) {
        size_t offset = pos - haystack_lower; // Calculate the offset to find the position in the original string
        result = (char *)haystack + offset; // Get the address in the original haystack string
    }

    // Free the allocated memory
    free(haystack_lower);
    free(needle_lower);

    return result; // Return the position of the match in the original haystack, or "NULL" if no match
}

int main() {
    struct Contact contacts[MAX_CONTACTS];  // Array to store up to MAX_CONTACTS contacts
    int count = 0;  // Initialize the contact count to 0 
    loadContactsFromFile(contacts, &count); // Load existing contacts from a file

    int choice;  // Variable to store user's menu choice
    while (1) { // Keep looping until the user decides to exit
        displayMenu();  // Display the menu options
        printf("Enter your choice: ");
        // Validate input to make sure the user enters a valid number
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input! Please enter a number.\n");
            while (getchar() != '\n');  // Clear the input buffer to avoid infinite loop
            continue; // Go back to the beginning of the loop and try again
        }

        // Handle menu choices with a switch statement 
        switch (choice) {
            case 1: addContact(contacts, &count); break;  // Add a new contact; calls the addContact function
            case 2: listContacts(contacts, count); break;  // List all contacts; calls the listContacts function
            case 3: searchContact(contacts, count); break;  // Search for a contact; calls the searchContact function
            case 4: editContact(contacts, count); break;   // Edit a contact's details; calls the editContact function
            case 5: deleteContact(contacts, &count); break;   // Delete a contact; calls the deleteContact function
            case 0: // User wants to exit
                printf("Exiting the program. Goodbye!\n");
                saveContactsToFile(contacts, count);  // Save contacts to a file before exiting
                return 0;  // Exit the program 
            default: 
                printf("Invalid choice! Please enter a valid option.\n");   // Invalid menu choice
        }
    }
}

// Display the main menu with options for the user
void displayMenu() {
    printf("\n\t\t***** Contact Management System *****\n"); // Menu Title
    printf("\t\t=====================================\n"); // Title seperator
    printf("\t\t[1] Add a New Contact\n"); // Option 1; add a contact
    printf("\t\t[2] List All Contacts\n"); // Option 2; list all contacts
    printf("\t\t[3] Search for a Contact\n"); // Option 3; search for a contact
    printf("\t\t[4] Edit a Contact\n"); // Option 4; edit a contact
    printf("\t\t[5] Delete a Contact\n"); // Option 5; delete a contact
    printf("\t\t[0] Exit\n"); // Option 0; exit the program
    printf("\t\t=====================================\n"); // Another seperator
}

// Add a new contact to the contact list
void addContact(struct Contact contacts[], int *count) {
    // Check if the contact list is already full
    if (*count >= MAX_CONTACTS) {
        printf("Contact list is full. Cannot add more contacts.\n");
        return; // Just exit the function if the list is full
    }
    while (getchar() != '\n'); // Clear any leftover characters in the input buffer

    // Get contact details from the user
    struct Contact *contact = &contacts[*count]; // Get a pointer to the next available slot in the contacts array
    int checkDuplicate = 0; // initialize variable to check if a duplicate name was added as we don't want duplicate names

    // Loop until we get a valid name
    do {
        printf("Enter contact name: ");
        fgets(contact->name, sizeof(contact->name), stdin); // Read the name from input
        contact->name[strcspn(contact->name, "\n")] = 0; // Remove newline from the name

        // Check if the entered name is empty
        if (strlen(contact->name) == 0) {
            printf("Name cannot be empty. Please enter a valid name.\n");
            continue; // Go back to the beginning of the loop and try again
        }

        checkDuplicate = 0; // Reset the duplicate flag
        // Check if the entered name already exists
        for (int i = 0; i < *count; i++) {
            if (strcasecmp(contact->name, contacts[i].name) == 0) { // Use strcasecmp for case-insensitive comparision
                printf("A contact with this name already exists. Please Enter a New Name:\n");
                checkDuplicate = 1; // Set the duplicate flag
                break; // No need to continue checking once a duplicate is found
            }
        }
    } while (strlen(contact->name) == 0 || checkDuplicate == 1); // Keep looping if the name is empty or a duplicate

    int validNumber = 0; // Set up a validity checker; we want a valid phone number

    while (!validNumber) {
        printf("Enter phone number: ");
        validNumber = 1; // Assume the number is valid until proven otherwise

        fgets(contact->phone, sizeof(contact->phone), stdin);  // Read phone number as string
        contact->phone[strcspn(contact->phone, "\n")] = 0;     // Remove trailing newline

        // Check if the phone number is empty
        if (strlen(contact->phone) == 0) {
            printf("Phone number cannot be empty.\n");
            validNumber = 0;
            continue;
        }

        // Check for allowed characters: digits, '+', and leading '00'
        int i = 0;
        if (contact->phone[0] == '+') {
            i = 1; // Skip the '+' sign
        } else if (contact->phone[0] == '0' && contact->phone[1] == '0') {
            i = 2; // Skip the leading zeros
        }

        for (; contact->phone[i] != '\0'; i++) {
            if (!isdigit(contact->phone[i])) {
                printf("Invalid characters in phone number. Please enter number in the format +XXXXXXXXXXXX or 00XXXXXXXXXXXX or XXXXXXXXXXXX\n");
                validNumber = 0;
                break;
            }
        }

        // Check for duplicate phone number if input is otherwise valid
        if (validNumber) {
            for (int i = 0; i < *count; i++) {
                if (strcmp(contacts[i].phone, contact->phone) == 0) { // Compare strings
                    printf("This phone number already exists please try again:\n");
                    validNumber = 0; // Duplicate found
                    break;
                }
            }
        }
    }

    printf("Enter address: ");
    fgets(contact->address, sizeof(contact->address), stdin); // Read the address from input
    contact->address[strcspn(contact->address, "\n")] = 0; // Remove newline from the address

    int validEmail = 0; // Check validity of email
    // Loop until we get a valid email
    while (!validEmail) {
        printf("Enter email: ");
        fgets(contact->email, sizeof(contact->email), stdin); // Takes input 
        contact->email[strcspn(contact->email, "\n")] = 0;   // Remove newline character 

        // Check if the entered email is empty
        if (strlen(contact->email) == 0) {
            printf("Email cannot be empty!\n");
            continue; // Go back to the beginning of the loop and try again
        }

        // Simple check to ensure both '@' and '.' are present in the email
        if (strchr(contact->email, '@') != NULL && strchr(contact->email, '.') != NULL) {
            validEmail = 1; // Email is valid 
        } else {
            printf("Invalid email! Email must contain both '@' and '.'.\n");
        }
    }

    (*count)++;  // Increment the contact count as a new contact was added
    printf("Contact added successfully!\n");
}

// List all contacts in the address book 
void listContacts(struct Contact contacts[], int count) {
    // Check if there are any contacts to display
    if (count == 0) {
        printf("No contacts to display.\n");
        return; // Exits the function
    }

    // Sort the contacts alphabetically by name before displaying
    qsort(contacts, count, sizeof(struct Contact), compareContacts);

    // Display the contact details in a formatted manner
    printf("\n\t\t*** List of Contacts ***\n");
    printf("=====================================================================\n");
    printf("Name\t\t\tPhone\t\t\tAddress\t\t\tEmail\n"); // This is the header row
    printf("=====================================================================\n");

    for (int i = 0; i < count; i++) {
        printf("%-30s %-20s %-30s %s\n", contacts[i].name, contacts[i].phone, contacts[i].address, contacts[i].email); // Print each contact's details
    }
}

// Comparison function for sorting contacts alphabetically (used by the qsort function)
int compareContacts(const void *a, const void *b) {
    // Cast the void pointers to Contact pointers
    const struct Contact *contactA = (const struct Contact *)a;
    const struct Contact *contactB = (const struct Contact *)b;
    return strcasecmp(contactA->name, contactB->name); // Compare the names using strcasecmp (case-insensitive)
}

// Search for a contact by name, or partial substring search
void searchContact(struct Contact contacts[], int count) {
    // Check if there are any contacts to search
    if (count == 0) {
        printf("No contacts to search.\n");
        return; // Nothing to search so it exits the function
    }

    // Ensures alphabetical order when searching
    qsort(contacts, count, sizeof(struct Contact), compareContacts); // Sort contacts alphabetically by name

    // reads the name and does a substring search; prompt the user to enter the name (or part of the name) to search for
    getchar(); // Clear any leftover characters in the input buffer
    char input[MAX_NAME_LEN]; // Array to store the user's input
    printf("Please enter the contact's name: ");
    fgets(input, sizeof(input), stdin); // Read the input from the user
    input[strcspn(input, "\n")] = 0; // Remove newline from the input

    // Check if the user entered anything
    if (strlen(input) == 0) {
        printf("No input provided. Returning.\n");
        return; // Nothing to search so it exits the function
    }

    int found = 0; // Flag to indicate if any matching contacts were found
    // Loop through all the contacts
    for (int i = 0; i < count; i++) {
        // partial substring search to check if the input is a substring of the contact's name (case-insensitive)
        if (caseInsensitiveStrStr(contacts[i].name, input) != NULL) {
            if (!found) { // If this is the first match, print a header
                // Print the contact details if found
                printf("\nContacts matching '%s':\n", input);
            }
            found = 1; // Set the found flag to 1
            // Print the contact's details
            printf("Name: %s\nPhone: %s\nAddress: %s\nEmail: %s\n\n", // Phone is now a string (%s)
                   contacts[i].name, contacts[i].phone, contacts[i].address, contacts[i].email);
        }
    }

    // If no matching contacts were found
    if (!found) {
        printf("No contact found containing '%s'.\n", input);
    }
}

// Edit an existing contact
void editContact(struct Contact contacts[], int count) {
    char name[MAX_NAME_LEN]; // Array to store the name of the contact to edit
    printf("Enter the name of the contact to edit: ");
    getchar(); // Clear any leftover characters in the input buffer
    fgets(name, sizeof(name), stdin); // Read the name from the user
    name[strcspn(name, "\n")] = 0; // Remove newline from the name

    // Search for the contact to edit
    for (int i = 0; i < count; i++) {
        // If the contact is found (case-insensitive comparison)
        if (strcasecmp(contacts[i].name, name) == 0) {
            printf("Editing contact '%s'\n", contacts[i].name);
            // Get new contact details from the user
            printf("New Name: ");
            fgets(contacts[i].name, sizeof(contacts[i].name), stdin); // Read the new name
            contacts[i].name[strcspn(contacts[i].name, "\n")] = 0; // Remove newline from the name

            printf("New Phone: ");
            fgets(contacts[i].phone, sizeof(contacts[i].phone), stdin); // Read new phone as string 
            contacts[i].phone[strcspn(contacts[i].phone, "\n")] = 0;     // Remove trailing newline 

            printf("New Address: ");
            fgets(contacts[i].address, sizeof(contacts[i].address), stdin); // Read the new address
            contacts[i].address[strcspn(contacts[i].address, "\n")] = 0; // Remove newline from the address

            printf("New Email: ");
            fgets(contacts[i].email, sizeof(contacts[i].email), stdin); // Read the new email
            contacts[i].email[strcspn(contacts[i].email, "\n")] = 0; // Remove newline from the email
            printf("Contact updated successfully!\n");
            return; // Exit the function after editing the contact
        }
    }
    printf("No contact found with the name '%s'.\n", name); // Contact not found message
}

// Delete a contact from the list
void deleteContact(struct Contact contacts[], int *count) {
    char name[MAX_NAME_LEN]; // Array to store the name of the contact to delete
    printf("Enter the name of the contact to delete: ");
    getchar(); // Clear any leftover characters in the input buffer
    fgets(name, sizeof(name), stdin); // Read the name from the user
    name[strcspn(name, "\n")] = 0; // Remove newline from the name

    // Find and delete the contact 
    for (int i = 0; i < *count; i++) {
        // If the contact is found (case-insensitive comparision)
        if (strcasecmp(contacts[i].name, name) == 0) {
            // Shift the remaining contacts to remove the deleted one, which moves all subsequent contacts one position up
            for (int j = i; j < *count - 1; j++) {
                contacts[j] = contacts[j + 1];
            }
            (*count)--; // Decrement the contact count as there is one less contact now
            printf("Contact deleted successfully.\n");
            return; // Exit the function after deleting the contact
        }
    }
    printf("No contact found with the name '%s'.\n", name); // Contact not found message
}

// Save all contacts to a file; this function is called when the user exits the program
void saveContactsToFile(struct Contact contacts[], int count) {
    FILE *fp = fopen("contacts.txt", "w"); // Open the file in write mode ("w")
    // Check if the file was opened successfully
    if (!fp) {
        printf("Failed to open file for saving contacts.\n");
        return; // Exit the function if the file could not be opened
    }

    // Loop through all contacts and write their details to the file
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s\n%s\n%s\n%s\n", contacts[i].name, contacts[i].phone, contacts[i].address, contacts[i].email); // Write each field on a new line
    }
    fclose(fp); // Close the file
    printf("Contacts saved to file.\n");
}

// Load contacts from a file into the array; this function is called when the program first starts
void loadContactsFromFile(struct Contact contacts[], int *count) {
    FILE *fp = fopen("contacts.txt", "r"); // Open the file in read mode ("r")
    // Check if the file exists or can be opened
    if (!fp) {
        printf("No previous contacts file found. Starting fresh.\n");
        return; // No file found, just exit the function
    }

    // Read contacts from the file until the end of the file is reached
    while (fscanf(fp, "%[^\n]\n%[^\n]\n%[^\n]\n%[^\n]\n", contacts[*count].name, contacts[*count].phone, contacts[*count].address, contacts[*count].email) != EOF) {
        (*count)++; // Increment the contact count for each contact read
    }
    fclose(fp); // Close the file~
}




