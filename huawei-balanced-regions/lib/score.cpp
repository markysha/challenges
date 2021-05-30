#include "common.cpp"

int main(int argc, char* argv[]) {
    ios_base::sync_with_stdio(0);

    string inputfile(argv[1]);
    string outputfile(argv[2]);

    ifstream input(inputfile);
    ifstream output(outputfile);

    int t;
    input >> t;
    
    int score_sum = 0;
    while (t--) {
        Input in;
        input >> in;
        utils::Checker checker(in);
        Output out;
        output >> out;
        int score = checker.score(out, true);
        // cout << "Test case score " << score << endl;
        score_sum += score;
    }
    cout << "SCORE = " << score_sum << endl;
}
