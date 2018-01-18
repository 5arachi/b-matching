all:
	g++ -Wall -pedantic -std=c++11 -Wno-reorder -O3 -I/Users/linhares/Applications/IBM/ILOG/CPLEX_Studio1261/cplex/include/ilcplex \
	/Users/linhares/Applications/IBM/ILOG/CPLEX_Studio1261/cplex/lib/x86-64_osx/static_pic/libcplex.a \
	BidirectedEdge.cpp BidirectedGraph.cpp BidirectedMatchingInstance.cpp CapacitatedbMatchingInstance.cpp \
	CapacitatedEdge.cpp CapacitatedGraph.cpp Constraint.cpp CO759lp.cpp Edge.cpp FlowEdge.cpp FlowGraph.cpp \
	main.cpp misc.cpp Settings.cpp Timer.cpp -o main -lpthread
