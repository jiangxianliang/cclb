#include <iostream>
#include <fstream>
using namespace std;


int main() {
	ifstream f;
	f.open("topo.txt");
	string buf;
	while (!f.eof()) {
			getline(f,buf);
		cout << "len " << buf.length() <<endl;
	
	}


}

	string buffer;
	printf("COMES1");
	ifstream fin;
	fin.open("topo.txt");
	if (fin.fail()){
		printf("TOPOLOGY FILE NOT FOUND");
	}
	printf("COMES2");
	int temp;
	int temp1;
	int temp2;
	int is_node = 1;
	while (!fin.eof()){

		getline(fin,buffer);
		printf("BUFFER LENGTH");
		printf("%d\n",buffer.length());
		if(buffer.length()==0)
			continue;
		if(buffer.compare("Links")==0){
			printf("found\n");
			is_node = 0;
			continue;
		}
		if(is_node == 1){
			temp = atoi(buffer.c_str());
			Topo_nodes n1;
			n1.node_num = temp;
			topo_nodes.push_back(n1);
			printf("COMES3");
		}
		if(is_node==0){
			temp = atoi(buffer.c_str());
			getline(fin,buffer,' ');
			temp1 = atoi(buffer.c_str());
			getline(fin,buffer,' ');
			temp2 = atoi(buffer.c_str());
			Topo_links l1;
			l1.n_start = temp;
			l1.n_end = temp1;
			l1.link_num = temp2;
			topo_links.push_back(l1);
		}


		
	for (int i=0;i<3;i++) {
		Topo_nodes n1;
		n1.node_num = i;
		topo_nodes.push_back(n1);
	}
	Topo_links l1 = Topo_links(0,1,0);
	Topo_links l2 = Topo_links(1,0,1);
	Topo_links l3 = Topo_links(1,2,2);
	Topo_links l4 = Topo_links(2,1,3);
	
	topo_links.push_back(l1);
	topo_links.push_back(l2);
	topo_links.push_back(l3);
	topo_links.push_back(l4);

