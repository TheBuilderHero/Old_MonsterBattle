/* A simple server in the internet domain using TCP
   The port number is passed as an argument
   This version runs forever, forking off a separate
   process for each connection
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>

void dostuff(int); /* function prototype */
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, pid;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;

     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0)
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     while (1) {
         newsockfd = accept(sockfd,
               (struct sockaddr *) &cli_addr, &clilen);
         if (newsockfd < 0)
             error("ERROR on accept");
         pid = fork();
         if (pid < 0)
             error("ERROR on fork");
         if (pid == 0)  {
             close(sockfd);
             dostuff(newsockfd);
             exit(0);
         }
         else close(newsockfd);
     } /* end of while */
     close(sockfd);
     return 0; /* we never get here */
}

/******** DOSTUFF() *********************
 There is a separate instance of this function
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/
void dostuff (int sock)
{
        std::string userName;
        std::string victoriesO;
        std::string defeatsO;
        int n;
        char buffer[256];
        std::string delimiter = "~~~";
        bzero(buffer,256);
        n = read(sock,buffer,255);
        if (n < 0) error("ERROR reading from socket");
        std::string str_file_content;
        std::string s = buffer;
        printf("Here is the message: %s\n",buffer);
        size_t saveOpenCheck = s.find(delimiter);
        size_t pos = 0;
        std::string token;

	//log the whole message recieved
	std::ofstream ilog;
	ilog.open("/home/kotaadmin/monsterBattle/logs/serverlog.dat", std::ios::app);
	ilog << std::to_string(sock) << " -> "<< std::string(buffer) << std::endl;
	ilog.close();

   if (std::string(buffer).substr(0, std::string(buffer).find(delimiter)) == "0"){
        s.erase(0, saveOpenCheck + delimiter.length());
        //run the code for saving

        std::ofstream myFile_Handler;
        //Open file in the directory /home/kotaadmin/userDate with the name of the user
        std::string str_file_name = std::string("/home/kotaadmin/monsterBattle/userData/") + std::string(s).substr(0, std::string(s).find(delimiter)) + std::string(".dat");
        myFile_Handler.open(str_file_name);  // no longer using this append instead we are starting from beginning each time opened
        //put data into file
        //Saving each peiece of data on its own line
        while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        myFile_Handler << token << std::endl;
        str_file_content += std::string(token) + std::string(" ");
        s.erase(0, pos + delimiter.length());
        }

        //Outputting the value of str_file_content to look a little nicer and remove the delimiter character
        std::string message = std::string(" Conneced to Server SAVING <") + str_file_content + s + std::string(">");  //had been outputting buffer instead to troubleshoot and get full picture of what I was getting
        n = write(sock, message.c_str(), sizeof(message) + sizeof(buffer));
        if (n < 0) error("ERROR writing to socket");

        myFile_Handler << s << std::endl;
        //File close
        myFile_Handler.close();

	//log the whole message recieved
/*	std::ofstream ilog;
	ilog.open("/home/kotaadmin/logs/serverlog.dat", std::ios::app);
	ilog << buffer;
	ilog.close();
*/

        //notify user their data was saved
        std::string prg_saved = std::string(" Progress Saved!");
        n += write(sock, prg_saved.c_str(), sizeof(prg_saved) + sizeof(buffer));
        if (n < 0) error("ERROR writing to socket");
   } else if (std::string(buffer).substr(0, std::string(buffer).find(delimiter)) == "1") {
        s.erase(0, saveOpenCheck + delimiter.length());

        std::ifstream ifile;
        //run the code for opening
        std::string filename = std::string("/home/kotaadmin/monsterBattle/userData/") + std::string(s).substr(0, std::string(s).find(delimiter)) + ".dat";
        ifile.open(filename);
        //read the data from file.
        ifile >> userName;
        ifile >> victoriesO;
        ifile >> defeatsO;
        //close ifile
        ifile.close();

	//log the whole message recieved
/*	std::ofstream ilog;
	ilog.open("/home/kotaadmin/logs/serverlog.dat", std::ios::app);
	ilog << std::string(buffer);
	ilog.close();
*/

        std::string message = std::string(" Conneced to Server VERIFYING <") + delimiter + userName + delimiter + victoriesO + delimiter + defeatsO + delimiter + std::string(">");
        n = write(sock, message.c_str(), sizeof(message) + sizeof(userName) + sizeof(victoriesO) + sizeof(defeatsO));
        if (n < 0) error("ERROR writing to socket");
   } else if (std::string(buffer).substr(0, std::string(buffer).find(delimiter)) == "2"){
        //return the the current saved data for review by User
        s.erase(0, saveOpenCheck + delimiter.length());

        std::ifstream ifile;
        //run the code for opening
        std::string filename = std::string("/home/kotaadmin/monsterBattle/userData/") + std::string(s).substr(0, std::string(s).find(delimiter)) + ".dat";
        ifile.open(filename);
        //read the data from file.
        ifile >> userName;
        ifile >> victoriesO;
        ifile >> defeatsO;
        //close ifile
        ifile.close();

	//log the whole message recieved
/*	std::ofstream ilog;
	ilog.open("/home/kotaadmin/logs/serverlog.dat", std::ios::app);
	ilog << buffer;
	ilog.close();
*/
        std::string message = std::string(" Conneced to Server OPENING <") + delimiter + userName + delimiter + victoriesO + delimiter + defeatsO + delimiter + std::string(">");
        n = write(sock, message.c_str(), sizeof(message) + sizeof(buffer));
        if (n < 0) error("ERROR writing to socket");
   } else {
        // Exit with nothing done.


	//log the whole message recieved
/*	std::ofstream ilog;
	ilog.open("/home/kotaadmin/logs/serverlog.dat", std::ios::app);
	ilog << buffer;
	ilog.close();
*/
   }
}


