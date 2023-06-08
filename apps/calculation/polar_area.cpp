#include <iostream>
#include <cmath>
#include "exprtk.hpp"
#include "polar_area.h"

using namespace std;

double calculateArea(const exprtk::expression<double>& polarCurve, double theta1, double theta2) {
    double area = 0.0;
    double deltaTheta = 0.001;

    for (double theta = theta1; theta <= theta2; theta += deltaTheta) {
        polarCurve.value();
        double r = polarCurve.value();
        double nextR = polarCurve.value(theta + deltaTheta);
        double sectorArea = 0.5 * (r * r + nextR * nextR) * deltaTheta;
        area += sectorArea;
    }

    return area;
}

int main() {
    string expression;
    cout << "Enter the polar curve function (use variable 'theta'): ";
    getline(cin, expression);

    exprtk::symbol_table<double> symbolTable;
    symbolTable.add_variable("theta", 0.0);
    symbolTable.add_constants();

    exprtk::expression<double> polarCurve;
    polarCurve.register_symbol_table(symbolTable);

    exprtk::parser<double> parser;
    parser.compile(expression, polarCurve);

    double theta1, theta2;
    cout << "Enter the starting angle (in radians): ";
    cin >> theta1;
    cout << "Enter the ending angle (in radians): ";
    cin >> theta2;

    double area = calculateArea(polarCurve, theta1, theta2);

    cout << "Area between the angles: " << area << endl;

    return 0;
}