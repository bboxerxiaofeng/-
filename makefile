 #########################################                                                                                                                                                 
ORACLE_HOME=/oracle/home
    
# oracle头文件路径
ORAINCL = -I$(ORACLE_HOME)/rdbms/public 
   
# oracle库文件路径
ORALIB =  -L$(ORACLE_HOME)/lib -L.
    
# oracle的oci库
ORALIBS = -lclntsh 
   
#CFLAGS = -O2
# CFLAGS = -O2 -Wall
   
CFLAGS = -g -Wall -Wno-write-strings #-Wno-unused-variable
  
all:socket_fork_business_client socket_fork_business_server

socket_fork_business_server:socket_fork_business_server.cpp /home/xiaofeng/Test/Model/freecplus_20200926/freecplus/_freecplus.h /home/xiaofeng/Test/Model/freecplus_20200926/freecplus/_freecplus.cpp sql.cpp sql.h _ooci.h
	g++ $(CFLAGS) -o socket_fork_business_server socket_fork_business_server.cpp /home/xiaofeng/Test/Model/freecplus_20200926/freecplus/_freecplus.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) _ooci.cpp sql.cpp sql.h _ooci.h -lm -lc

socket_fork_business_client:socket_fork_business_client.cpp /home/xiaofeng/Test/Model/freecplus_20200926/freecplus/_freecplus.h /home/xiaofeng/Test/Model/freecplus_20200926/freecplus/_freecplus.cpp
	g++ $(CFLAGS)-o socket_fork_business_client socket_fork_business_client.cpp /home/xiaofeng/Test/Model/freecplus_20200926/freecplus/_freecplus.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) _ooci.cpp -lm -lc


