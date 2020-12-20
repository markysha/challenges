#include "common.cpp"

int main(int argc, char* argv[]) {
    ios_base::sync_with_stdio(0);

    string inputfile(argv[1]);
    string outputfile(argv[2]);

    ifstream input(inputfile);
    ifstream output(outputfile);

    Input in;
    input >> in;
    Output out;
    output >> out;
    long double score = utils::score(in, out);

    cout << fixed << setprecision(8);
    cout << "SCORE = " << score << endl;
}
