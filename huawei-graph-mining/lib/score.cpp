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
    Score score = utils::score(in, out);

    cout << fixed << setprecision(8);
    cout << "modularity: " << score.modularity << endl;
    cout << "regularization: " << score.regularization << endl;
    cout << "SCORE = " << score.modularity + score.regularization << endl;
}
