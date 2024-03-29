﻿#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<sstream>
#include<unordered_map>

using namespace std;

//global variables

//declaration of classes
struct Vertex {
	int first_index;
	int nr_seq;
	string oligo_sequence;

	friend ostream& operator<<(ostream& os, const Vertex& v) {
		os << "Seq number: " << v.nr_seq << "\tPosition: " << v.first_index << "\tSequence: " << v.oligo_sequence;
		return os;
	}
};

//declarations of functions
string give_file_name();
int give_threshold();
int give_substring_length();
void read_fasta_data(vector <char>(&fasta)[5], string file_name);
void read_qual_data(vector<int>(&qual)[5], string file_name);
void delete_below_threshold(vector<char>(&fasta)[5], vector<int>(&qual)[5], vector<char>(&sequence)[5], vector<int>(&input_sequence_index)[5], int threshold);
void create_graph(vector<vector<Vertex>> &graph, vector<char> sequence[5], vector<int> input_sequence_index[5], int substring_length);
void connect_edges(vector<vector<Vertex>>& graph, int substring_length);
vector<Vertex> find_result(vector<vector<Vertex>>graph);

//main project
int main() {
	string file_name;
	int threshold, substring_length;
	//vector<string> fasta[5];
	//vector<string> qual[5];
	vector<char> fasta[5];
	vector<int> qual[5];
	string line;
	vector<char> sequence[5];
	vector<int> input_sequence_index[5];
	vector<vector<Vertex>> graph; 

	file_name = give_file_name();
	threshold = give_threshold();
	substring_length = give_substring_length();

	read_fasta_data(fasta, file_name + ".fasta"); //reading data from fasta file
	read_qual_data(qual, file_name + ".qual"); //  reading data from qual file

	delete_below_threshold(fasta, qual, sequence, input_sequence_index, threshold); //deleting nucleotides from sequence below threshold
	// the data is now stored in vector of chars along with corresponding vector of input_sequence_index

	create_graph(graph, sequence, input_sequence_index, substring_length); // creating graph where vertex is an oligonucleotide sequence

	// connecting vertices using edges
	connect_edges(graph, substring_length);

	// find motive using naive algorithm
	vector<Vertex> result = find_result(graph);
	
	//wypisanie rezultatu na wyjściu w postaci: nr sekwencji wejściowej, 
	// nr pozycji w tej sekwencji, dla każdego podciągu wchodzącego w skład kliki (struktury zbliżonej do kliki)
	// oraz sekwencję nukleotydową podciągu

	cout << endl << "results: " << endl;
	for (const auto& vertex : result) {
		cout << vertex << endl;
	}
	cout << endl;

	//test
	//cout << substring_length << endl;
	//for (int i = 0; i < 5; i++) {
	//	for (const char& s : sequence[i]) {
	//		cout << s << endl;
	//		cout << endl;
	//	}
	//}
	//for (int i = 0; i < 5; i++) {
	//	cout << "fasta[" << i << "]: ";
	//	for (char c : fasta[i]) {
	//		cout << c << " ";
	//	}
	//	cout << endl;
	//}
	//for (int i = 0; i < 5; i++) {
	//	std::cout << "qual[" << i << "]: ";
	//	for (int j : qual[i]) {
	//		std::cout << j << " ";
	//	}
	//	std::cout << std::endl;
	//}
	//for (int i = 0; i < 5; i++) {
	//	cout << "sequence[" << i << "]: ";
	//	for (char c : sequence[i]) {
	//		cout << c << " ";
	//	}
	//	cout << endl;
	//}
	//for (int i = 0; i < 5; i++) {
	//	std::cout << "previous index[" << i << "]: ";
	//	for (int j : input_sequence_index[i]) {
	//		std::cout << j << " ";
	//	}
	//	std::cout << std::endl;
	//}

	//for (int i = 0; i < 5; i++) {
	//	cout << "instance [" << i << "]" << endl;
	//	for (int j = 0; j < fasta[i].size(); j++) {
	//		cout << "fasta: " << fasta[i][j] << "(" << qual[i][j] << ")[" << j + 1 << "] ";
	//	}
	//	cout << endl << "sequence[" << i << "]\tthreshold: " << threshold << endl;
	//	for (int k = 0; k < sequence[i].size(); k++) {
	//		cout << "char: " << sequence[i][k] << "[" << input_sequence_index[i][k] << "]" << "\t";
	//	}
	//	cout << endl;

	//}

	//for (const auto& vertices : graph) {
	//	for (const auto& vertex : vertices) {
	//		cout << vertex << " ";
	//	}
	//	cout << endl;
	//}

}

//definitions of functions
string give_file_name() { //function that prompts the user to name specific file
	string file_name;
	cout << "Please write full name of the instance file to read:";
	cin >> file_name;
	return file_name;
}

int give_threshold() {
	int threshold;
	cout << "Please provide a threshold that will determine reliable data: ";
	cin >> threshold;
	return threshold;
}

int give_substring_length() {
	int substring;
	cout << "Please specify the length of the substring from a sequence, with a range of 4 to 9: ";
	cin >> substring;
	if (substring < 4 || substring > 9) { //exception handler
		substring = give_substring_length();
	}
	return substring;

}

//void read_file_data(vector <string>(&fasta)[5], string file_name) {
//	string line;
//	ifstream instance(file_name);
//
//	if (instance.is_open()) { 
//		int i = 0;
//		while (getline(instance, line)) {
//			if (i == 5)
//				break;
//			if (line.find(">") == std::string::npos) {
//				fasta[i].push_back(line);
//				i++;
//			}
//		}
//	}
//}
void read_fasta_data(vector <char>(&fasta)[5], string file_name) {
	string line;
	ifstream instance(file_name); // fasta input
	if (!instance.is_open()) {
		cout << endl << "Failed to open fasta file" << endl;
	}
	else {
		int i = 0;
		while (getline(instance, line)) {
			if (i == 5)
				break;
			if (line.find(">") == string::npos) {
				for (char c : line) {
					fasta[i].push_back(c);
				}
				i++;
			}
		}
		instance.close();
	}
}
void read_qual_data(vector <int>(&qual)[5], string file_name) {
	string line;
	ifstream instance(file_name); // qual input
	if (!instance.is_open()) {
		cout << endl << "Failed to open qual file" << endl;
	}
	else {
		int i = 0;
		while (i < 5 && getline(instance, line)) {
			if (line.find(">") == string::npos) {
				istringstream iss(line);
				int value;
				while (iss >> value) {
					qual[i].push_back(value);
				}
				i++;
			}
		}
		instance.close();
	}
}

void delete_below_threshold(vector<char>(&fasta)[5], vector<int>(&qual)[5], vector<char>(&sequence)[5], vector<int>(&input_sequence_index)[5], int threshold) {
	for (int i = 0; i < 5; i++) { //save particular chars from original indexes which qual values are equal or above threshold
		int counter = 1;
		for (int j = 0; j < fasta[i].size(); j++) {
			if (qual[i][j] >= threshold) {
				sequence[i].push_back(fasta[i][j]);
				input_sequence_index[i].push_back(counter);
			}
			++counter;
		}
	}
}

void create_graph(vector<vector<Vertex>>&graph, vector<char> sequence[5], vector<int> input_sequence_index[5], int substring_length) {
	for (int i = 0; i < 5; i++) { // for each sequence
		int counter = 0; //at which nucleotide from sequence we currently are in
		while (counter < (sequence[i].size() - (substring_length - 1))) {	//Do as many times so you exhaust whole sequence, beware of substring frame
			Vertex oligonucleotide;
			oligonucleotide.first_index = input_sequence_index[i][counter]; // i - sequence number, counter - index of first nucleotide
			oligonucleotide.nr_seq = i;
			for (int k = 0; k < substring_length; k++) { // iterate as many times as there is substring_length
				oligonucleotide.oligo_sequence += sequence[i].at(counter++);
			}
			counter -= (substring_length - 1); // counter is set to one place higher after this operation as we progress with counter by increasing it by
			// one nucleotide
			graph.emplace_back(vector<Vertex>{oligonucleotide});
		}
	}
}

void connect_edges(vector<vector<Vertex> >& graph, int substring_length) {
	for (int i = 0; i < (graph.size() - 1); i++) { // "-1" so it wont compare the last element to non-existing element of this vector
		for (int j = i + 1; j < graph.size(); j++) {
			int distance = 0; // distance between oligonucleotides
			if (graph[i][0].first_index > graph[j][0].first_index) {
				distance = graph[i][0].first_index - graph[j][0].first_index;
			}
			else {
				distance = graph[j][0].first_index - graph[i][0].first_index;
			}
			if (graph[i][0].oligo_sequence == graph[j][0].oligo_sequence // if oligosequences are aligned
				&& graph[i][0].nr_seq != graph[j][0].nr_seq  // compare different sequences
				&& distance <= (substring_length * 10)) { // if oligonucleotides arent too far from each other
				graph[i].push_back(graph[j][0]); // graph[i][0] have edges to -> graph[i][j]
				graph[j].push_back(graph[i][0]);
			}
		}
	}
}

vector<Vertex> find_result(vector<vector<Vertex>>graph) {
	vector<Vertex> result;
	for (const auto& outerVec : graph) {
		unordered_map<int, Vertex> nrSeqToVertexMap;

		if (!result.empty()) {
			return result;
		}
		for (const auto& vertex : outerVec) {
			// Check if nr_seq is within the required range and is not already found
			if (nrSeqToVertexMap.count(vertex.nr_seq) == 0) {
				nrSeqToVertexMap[vertex.nr_seq] = vertex;
			}

			// Check if we have found one vertex for each nr_seq value
			if (nrSeqToVertexMap.size() == 5) {

				for (int i = 0; i < 5; ++i) {
					result.push_back(nrSeqToVertexMap[i]);
				}
				return result;
			}
		}
	}
	return result;
}