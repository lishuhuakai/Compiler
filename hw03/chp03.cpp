#include <iostream>
#include <set>
#include <vector>
using namespace std;

///////////////////////////////
// translate a re into NFA
// I hope the code was not so complicated
typedef struct trans{
	int vertex_from;
	int vertex_to;
	char trans_symbol;
}trans;

class NFA {
public:
	vector<int> vertex;
	vector<trans> edges;
	int final_state;
	NFA() {}
	void set_vertex(int size) {
		for (int i = 0; i < size; ++i) {
			vertex.push_back(i);
		}
	}

	void set_transition(int vertex_from, int vertex_to, char trans_symbol) {
		trans new_trans;
		new_trans.vertex_from = vertex_from;
		new_trans.vertex_to = vertex_to;
		new_trans.trans_symbol = trans_symbol;
		edges.push_back(new_trans);
	}

	void add_transition(trans& tr) {
		edges.push_back(tr);
	}

	int get_vertex_size() {
		return vertex.size();
	}

	int get_final_state() {
		return final_state;
	}

	void set_final_state(int state) {
		final_state = state;
	}

	void display() {
		trans new_trans;
		cout << "\n";
		for (size_t i = 0; i < edges.size(); i++) {
			new_trans = edges.at(i);
			cout << "q" << new_trans.vertex_from << " --> q" << new_trans.vertex_to << " : Symbol - " << new_trans.trans_symbol << endl;
		}
		cout << "\nThe final state is q" << get_final_state() << endl;
	}
};

NFA concat(NFA& a, NFA& b) {
	// first we should construct all the node 
	NFA result;
	result.set_vertex(a.get_vertex_size() + b.get_vertex_size());
	size_t i;
	trans new_trans;

	for (i = 0; i < a.edges.size(); ++i) {
		new_trans = a.edges.at(i);
		result.add_transition(new_trans);
	}

	result.set_transition(a.get_final_state(), a.get_vertex_size(), '^');

	for (i = 0; i < b.edges.size(); ++i) {
		new_trans = b.edges.at(i);
		new_trans.vertex_from += a.get_vertex_size();
		new_trans.vertex_to += a.get_vertex_size();
		result.add_transition(new_trans);
	}

	result.set_final_state(a.get_vertex_size() + b.get_vertex_size() - 1); // yes, we did it
	return result;
}

NFA kleene(NFA& a) {
	NFA result;
	size_t i;
	trans new_trans;

	result.set_vertex(a.get_vertex_size() + 2);
	result.set_transition(0, 1, '^');

	for (i = 0; i < a.edges.size(); ++i) {
		new_trans = a.edges.at(i);
		new_trans.vertex_from += 1;
		new_trans.vertex_to += 1;
		result.add_transition(new_trans);
	}

	result.set_transition(a.get_vertex_size(), a.get_vertex_size() + 1, '^');
	result.set_transition(a.get_vertex_size(), 1, '^');
	result.set_transition(0, a.get_vertex_size() + 1, '^');
	result.set_final_state(a.get_final_state() + 1);
	return result;
}

NFA or_selection(vector<NFA> selections, int no_of_selections) {
	NFA result;
	int vertex_count = 2;
	int i, j;
	NFA med;
	trans new_trans;

	for (i = 0; i < no_of_selections; ++i) {
		vertex_count += selections.at(i).get_vertex_size();
	}

	result.set_vertex(vertex_count);

	int adder_track = 1;

	for (i = 0; i < no_of_selections; ++i) {
		result.set_transition(0, adder_track, '^');
		med = selections.at(i);
		for (j = 0; j < med.edges.size(); ++j) {
			new_trans = med.edges.at(j);
			new_trans.vertex_from += adder_track;
			new_trans.vertex_to += adder_track;
			result.add_transition(new_trans);
		}
		adder_track += med.get_vertex_size();
		result.set_transition(adder_track - 1, vertex_count - 1, '^');
	}
	result.set_final_state(vertex_count - 1);
	return result;
}

int main_01() {
	NFA a, b;
	cout << "\nFor the regular expression segment : (a)" << endl;
	a.set_vertex(2);
	a.set_transition(0, 1, 'a');
	a.set_final_state(1);
	a.display();

	cout << "\nFor the regular expression segment : (b)";
	b.set_vertex(2);
	b.set_transition(0, 1, 'b');
	b.set_final_state(1);
	b.display();

	cout << "\nFor the regular expression segment [Concatenation] : (a.b)";
	NFA ab = concat(a, b);
	ab.display();

	cout << "\nFor the regular expression segment [Kleene Closure] : (a*)";
	NFA a_star = kleene(a);
	a_star.display();

	cout << "\nFor the regular expression segment [Or] : (a|b)";
	int no_of_selections;
	no_of_selections = 3;
	vector<NFA> selections(no_of_selections, NFA());
	NFA c;
	c.set_vertex(2);
	c.set_transition(0, 1, 'c');
	c.set_final_state(1);

	selections.at(0) = a;
	selections.at(1) = b;
	selections.at(2) = c;
	NFA a_or_b_or_c = or_selection(selections, no_of_selections);
	a_or_b_or_c.display();

	system("pause");
	return 0;
}