/* Test machine: csel-kh4250-16.cselabs.umn.edu
* Name: Babacar Diouf, Salahudin Farah, Rezkath Awal
* X500: diouf006, farah176, awalx003 */

-Group ID on Canvas: 79

-Refer to the steps below to compile and run our program:
	1. Open one terminal to simulate the server
	2. Type "make clean" and hit the enter key
	3. Type "make web_server" and hit the enter key
	4. Type 
		./web_server <port> <path_to_testing>/testing <num_dispatch> <num_worker> 1 <queue_len> <number_greater_than_or_equal_to_1> 
	and hit the enter key
	
	5. Now open another terminal to simulate the client
	6. Then to send one request at a time while seeing the responses and using non-persistent connections, type 
		wget -S --content-on-error --no-http-keep-alive http://127.0.0.1:<port>/<image_or_text>/<if_image:gif_or_jpg_else_if_text_html_or_plain_or_big.txt>/<number>.<file_extension> 
	and hit the enter key
	7. To send multiple requests simultaneously while seeing the responses and using non-persistent connections, type 
		cat <path_to_urls_file> | xargs -n 1 -P <number_of_requests_at_a_time> wget -S --content-on-error --no-http-keep-alive
	and hit the enter key

-Brief explanation on how your program works:  The way our program works is similar to the one in Project 3. The only difference in this project is we have to implement the networking utility functions by ourselves using POSIX socket programming. Thus, just like in Project 3, Our program takes a URL containing the path to a file the user wants to be downloaded.  The server takes that URL and downloads the desired file into a folder.

-Indicate if your group implemented the extra credit: No extra credit has been attempted.

-An explanation of your extra credit design choices and implementation:: No extra credit has been attempted.

-Contributions of each team member towards the project development:
	-Babacar Diouf
		-Helped write multiple functions
		-Helped debug part of the program
		-Helped with documentation
		-Helped write this README file
	-Salahudin Farah
		-Helped write multiple functions
		-Helped debug part of the program
		-Helped with documentation
		-Helped write Interim Report for first submission deadline
	-Rezkath Awal
		-Helped write multiple functions
		-Helped debug part of the program
		-Helped with documentation
		-Helped write Interim Report for first submission deadline
		-Helped write this README file
		
		





		
		
		
		


		
		
