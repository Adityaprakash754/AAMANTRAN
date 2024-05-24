#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include <Python.h>


// structure to hold all details of a node
typedef struct members
{
    char name[50];
    char response[4];
    char mail[100];
    int count;
} M;

// structure to create a linked list
struct node
{
    M info;
    struct node * link;
};
typedef struct node * NODE;

// to allocate memory location to a node 
NODE getNode()
{
    NODE x;
    x = (NODE)malloc(sizeof(struct node));
    if(x == NULL){
        printf("memory wasn't allocated \n");
        exit(0);
    }
    return x;
}

// to insert a node to the end of the linked list
NODE insert_rear(NODE guests,M item)
{
    NODE cur, temp = getNode();
    temp -> info = item;
    temp -> link = NULL;
    cur = guests;
    
    if(cur == NULL)
    return temp;
    
    while(cur -> link != NULL)
        cur = cur -> link;
    
    cur -> link = temp;
    return guests;
}

// count total number of nodes for bubble sort
int countNode(NODE guests)
{
    int c = 0;
    NODE curr;
    if(guests == NULL)
    return c;
    curr = guests;
    while(curr != NULL){
        c++;
        curr = curr -> link;
	}
    return c;

}
 
// to sort nodes based on NAMEs using bubble sort
void sort(NODE guests)
{
    int i, j, c;
    M temp; 
    NODE cur, next;

    c = countNode( guests );
 
    for( i = 0;i < c - 1; i++)
    {
        cur = guests;
        next = cur -> link;

        for( j = 0; j < c - i - 1; j++)
        {
            if(strcmp((cur -> info).name,(next -> info).name) > 0)
            {
                temp = cur -> info;
                cur -> info = next -> info;
                next -> info = temp;
            }
            cur = next;
            next = cur -> link;
        }
    }   	
}

// to delete first entry of multiple entries since bubble sort is stable and assuming later entry is correct
NODE delMulEntr(NODE guests)
{
    int i, c;
    c = countNode( guests );
    NODE prev, cur, next;
    prev = NULL;
    cur = guests;
    next = cur -> link;

    for( i = 0; i < c - 1; i++)
    {
        if(strcmp((cur -> info).name,(next -> info).name) == 0)
        {
            if(prev == NULL)
            {
                free(guests);
                guests = next;
            }
            else
                prev -> link = next;
            
        }
        else
            prev = cur; 

        cur = next;
        next = cur -> link; 
    }

    return guests;
}

// to read csv file to take input of all the entries
NODE readFile(NODE guests)
{
    // creating a file pointer and opening file in read mode
    FILE *fp;
    fp = fopen("PartyInvite.csv","r");

    if(fp == NULL)
    {
        printf("Error in opening Party Invite file.\n");
        exit(1);
    }

    // iterate till end of file
    while(!feof(fp)) 
    { 
        M item;

        // reading each item of the line sepearted by a comma
        fscanf(fp," %[^,], %[^,], %[^,],%d\n",item.name,item.response,item.mail,&item.count);

        // to convert first letter of name to upper case
        item.name[0] = toupper(item.name[0]); 

        //printf("%s %s %s %d\n", item.name, item.response, item.mail, item.count);

        // if the response is yes and no of people visiting != 0 then only insert them into the linked list
        if (strcmp(item.response,"Yes") == 0 && item.count != 0)
        guests = insert_rear( guests, item);
        
        if (ferror(fp))
        {
            printf("Error in reading the file.\n");
            exit(1);
        }
    } 
    fclose(fp);

    // for testing print all elements in linked list
   
    NODE cur = guests;
    while(cur != NULL)
    {
        printf("%s %s %s %d\n", (cur -> info).name, (cur -> info).response, (cur -> info).mail, (cur -> info).count);
        printf("\n");
        cur = cur -> link;
    }
    return guests; 
}


//  Embedding python into my c project to send an invitation using email
void sendMail(NODE guests)
{
//  to initialize the Python interpreter in C program.
    Py_Initialize();

    //  PyRun_SimpleString function from the Python C API to execute Python code snippets

    /*
    This line imports the smtplib module in Python. The smtplib module defines an SMTP client session object 
    that can be used to send email to any internet machine with an SMTP listener daemon.
    */
    PyRun_SimpleString("import smtplib");           

    /*
    This line creates an instance of the SMTP class from the smtplib module.
    It connects to the Gmail SMTP server (smtp.gmail.com) on port 587 (which is the default port for secure SMTP communication).
    */
    PyRun_SimpleString("smtpObj = smtplib.SMTP('smtp.gmail.com', 587)");

    /*
    This line sends an EHLO (Extended Hello) command to the SMTP server. The EHLO command is used by the client 
    to identify itself to the server and to obtain a list of the SMTP service extensions that the server supports. 
    This is a standard part of the SMTP protocol.
    */
    PyRun_SimpleString("smtpObj.ehlo()");

    /*
    This line initiates a TLS (Transport Layer Security) encrypted connection with the SMTP server. 
    The starttls() method is used to upgrade the existing insecure connection to a secure (encrypted) connection. 
    This step is crucial for secure communication when sending emails over the internet.
    */
    PyRun_SimpleString("smtpObj.starttls()");

    /*
    This line calls the login method of the smtpObj (SMTP object) to authenticate with the Gmail SMTP server. 
    The login method takes two arguments: the email address ('xyz@gmail.com') and the corresponding password ('PASSWORD').
    */
    PyRun_SimpleString("smtpObj.login('XYZ@gmail.com', 'PASSWORD')");

    /*
    In this multiline string:
    'Subject: Greetings!!! you have been invited' is the subject of the email.
    The first newline (\n) creates a new line in the email body.
    'I am very pleased to invite you to my partyyyyy!!!!' is the main content of the email.
    */
    PyRun_SimpleString("text = '''Subject: Greetings!!! you have been invited\n\nI am very pleased to invite you to my partyyyyy!!!!'''");
    
    while (guests != NULL) {

        // storing the data from linked list into a character pointer
        char* to = (guests->info).mail;

        /*
        creating a Python Unicode object from the C string . This is necessary because Python operates with Unicode strings, 
        and you need to convert the C string to a Python Unicode object for further processing in Python.
        */
        PyObject *to_str = PyUnicode_FromString(to);

        /*
        Here, we are extracting the UTF-8 encoded version of the email address from the Python Unicode object. 
        PyUnicode_AsUTF8 is a Python C API function that retrieves the UTF-8 representation of a Unicode object.
        */
        const char *recipient_email = PyUnicode_AsUTF8(to_str);

        char script[1000]; // Adjust the size according to need

        /*
        We are using snprintf to format the Python script we want to execute. 
        The formatted string includes the recipient's email address (recipient_email) obtained from the PyUnicode_AsUTF8 conversion. 
        The email is sent from 'xyz@gmail.com'. text is a Python variable containing the email body. 
        This formatted string will be used with PyRun_SimpleString to execute the Python code within your C program.
        */
        snprintf(script, sizeof(script), "smtpObj.sendmail('XYZ@gmail.com', '%s', text)", recipient_email);

        // Pass the formatted string to PyRun_SimpleString
        PyRun_SimpleString(script);
        guests = guests->link;

    }

    PyRun_SimpleString("smtpObj.quit()");
 
    
    Py_Finalize();
 
}

int main()
{
    NODE guests = NULL;
    guests = readFile( guests );

// sorting members based on names
    sort(guests);

    // for testing print all elements in linked list
    NODE cur = guests;
    while(cur != NULL)
    {
        printf("%s %s %s %d\n", (cur -> info).name, (cur -> info).response, (cur -> info).mail, (cur -> info).count);
        printf("\n");
        cur = cur -> link;
    }

// to delete first entry of multiple entries 
    guests = delMulEntr( guests);

    // for testing print all elements in linked list
    cur = guests;
    while(cur != NULL)
    {
        printf("%s %s %s %d\n", (cur -> info).name, (cur -> info).response, (cur -> info).mail, (cur -> info).count);
        printf("\n");
        cur = cur -> link;
    }

// to send invitation email/ pass to everyone who are intrested to come to party
    sendMail(guests);

// to deallocate the linked list    
    free(guests);
    
    return 0;
}