import assert from 'node:assert/strict';

import InstantiatePoincare from './poincare.mjs';

console.log('> Starting tests\n');

let nTests = 0;
let nSuccess = 0;

async function testCase(featureName, testFunction) {
  // Create a new Poincare instance for each test
  const poincare = await InstantiatePoincare();
  nTests += 1;
  let success = true;
  let error = null;
  try {
    await testFunction(poincare);
    nSuccess += 1;
  } catch (e) {
    error = e;
    success = false;
  }
  console.log((success ? '✅ ' : '❌ ') + 'Test: ' + featureName);
  if (!success) {
    console.log(error);
  }
}

Promise.all([
  // Wait for all tests to complete before logging end message
  testCase('Module - runAndCleanup', (poincare) => {
    var expression;
    poincare.runAndCleanup(() => {
      expression = poincare.PCR_UserExpression.BuildFromLatex('1/2');
    });
    // The expression should be deleted after the callback
    assert.ok(expression.isDeleted());
  }),
  testCase('Regression - Linear', (poincare) => {
    var series = new poincare.PCR_RegressionSeries(
      [1.0, 8.0, 14.0, 79.0],
      [-3.581, 22.2, 40.676, 261.623],
    );

    assert.equal(series.getXArray()[0], 1.0);

    assert.equal(series.numberOfPairs(), 4);
    assert.equal(series.slope(), 3.3910000000000005);

    var regression = new poincare.PCR_Regression(poincare.RegressionType.LinearAxpb);

    var hasR = regression.hasR();
    assert.ok(hasR);
    var hasRSquared = regression.hasRSquared();
    assert.ok(hasRSquared);
    var hasR2 = regression.hasR2();
    assert.ok(!hasR2);

    var formula = regression.latexTemplateFormula();
    assert.equal(formula, 'a\\cdot x+b');

    var coefficients = regression.fit(series);
    assert.deepEqual(coefficients, [3.3910000000000005, -6.241000000000014, NaN, NaN, NaN]);
    var prediction = regression.evaluate(coefficients, 10);
    assert.equal(prediction, 27.66899999999999);

    var r = regression.correlationCoefficient(series);
    assert.equal(r, 0.9999713636007523);
    var r2 = regression.determinationCoefficient(series, coefficients);
    assert.equal(r2, 0.999942728021548);
    var residual = regression.residualAtIndex(series, coefficients, 0);
    assert.equal(residual, -0.7309999999999865);
    var residualStdev = regression.residualStandardDeviation(series, coefficients);
    assert.equal(residualStdev, 1.1334028410057904);
  }),

  testCase('Statistics - Array', (poincare) => {
    var dataset = new poincare.PCR_StatisticsDataset([1.0, 5.0, -14.0, 10.0]);
    assert.equal(dataset.mean(), 0.5);
  }),

  testCase('Statistics - Series', (poincare) => {
    var dataset = new poincare.PCR_StatisticsDataset([1.0, 5.0, -14.0, 10.0], [2.0, 4.0, 1.0, 3.0]);
    assert.equal(dataset.getValuesArray().length, 4);
    assert.equal(dataset.mean(), 3.8);
    assert.equal(dataset.variance(), 45.36);
    assert.equal(dataset.median(), 5.0);
    assert.equal(dataset.max(), 10.0);
  }),

  testCase('Expression - Parse, Reduce, Approximate', (poincare) => {
    const userExpression = poincare.PCR_UserExpression.BuildFromLatex('\\frac{6}{9}');

    assert.ok(!userExpression.isUninitialized());

    const emptyContext = new poincare.PCR_EmptyContext();
    const reductionContext = poincare.PCR_ReductionContext.Default(emptyContext, false);

    const reducedExpression = userExpression.cloneAndReduce(reductionContext);
    assert.ok(!reducedExpression.isUninitialized());

    const userReducedExpression = reducedExpression.cloneAndBeautify(reductionContext);
    assert.ok(!userReducedExpression.isUninitialized());
    const numberOfSignificantDigits = 7;
    // We don't care about the withThousandsSeparators value, they won't be added either way.
    const withThousandsSeparators = true;
    assert.equal(
      userReducedExpression.toLatex(numberOfSignificantDigits, withThousandsSeparators),
      '\\frac{2}{3}',
    );

    const userApproximateExpression = reducedExpression
      .approximateToTree()
      .cloneAndBeautify(reductionContext);

    assert.ok(!userApproximateExpression.isUninitialized());
    assert.equal(
      userApproximateExpression.toLatex(numberOfSignificantDigits, withThousandsSeparators),
      '0.6666667',
    );
  }),

  testCase('Expression - System Function, Derivative', (poincare) => {
    const userExpression = poincare.PCR_UserExpression.BuildFromLatex('x^{2}-2x+1');

    assert.ok(!userExpression.isUninitialized());

    const emptyContext = new poincare.PCR_EmptyContext();
    const reductionContext = poincare.PCR_ReductionContext.Default(emptyContext, true);

    const reducedExpression = userExpression.cloneAndReduce(reductionContext);
    assert.ok(!reducedExpression.isUninitialized());

    const systemFunction = reducedExpression.getSystemFunction('x');
    assert.ok(!systemFunction.isUninitialized());
    assert.equal(systemFunction.approximateToScalarWithValue(3), 4);

    const firstDerivative = reducedExpression
      .getReducedDerivative('x', 1)
      .cloneAndBeautify(reductionContext);
    assert.ok(!firstDerivative.isUninitialized());

    const numberOfSignificantDigits = 7;
    // We don't care about the withThousandsSeparators value, they won't be added either way.
    const withThousandsSeparators = true;
    assert.equal(
      firstDerivative.toLatex(numberOfSignificantDigits, withThousandsSeparators),
      '2x-2',
    );

    const secondDerivative = reducedExpression
      .getReducedDerivative('x', 2)
      .cloneAndBeautify(reductionContext);
    assert.ok(!secondDerivative.isUninitialized());
    assert.equal(secondDerivative.toLatex(numberOfSignificantDigits, withThousandsSeparators), '2');

    const lowerBound = poincare.PCR_SystemExpression.BuildInt(0);
    const upperBound = poincare.PCR_SystemExpression.BuildInt(1);
    const integral = systemFunction.approximateIntegralToScalar(lowerBound, upperBound);
    assert.equal(integral, 0.3333333333333333);
  }),

  testCase('Expression - Retrieve tree from CPP heap', async (poincare) => {
    const expression = poincare.PCR_UserExpression.BuildFromLatex('1+2');
    assert.ok(!expression.isUninitialized());
    const storedTree = expression.getTree();
    const expectedTree = new UserExpressionTree([19, 2, 6, 7]);
    assert.deepEqual(storedTree, expectedTree);

    // Reinstantiate in a new Poincare
    const newPoincare = await InstantiatePoincare();
    const newExpression = newPoincare.PCR_UserExpression.BuildFromTree(storedTree);
    assert.ok(!newExpression.isUninitialized());
    assert.equal(newExpression.toLatex(7, true), '1+2');
  }),

  testCase('Expression builder', (poincare) => {
    const expression = poincare.PCR_UserExpression.BuildFromPattern(
      'Pow(Add(Mult(MinusOne,K0),K1),Pi)',
      poincare.PCR_UserExpression.BuildInt(2),
      poincare.PCR_UserExpression.BuildFloat(1e-3),
    );
    assert.ok(!expression.isUninitialized());
    const numberOfSignificantDigits = 7;
    // We don't care about the withThousandsSeparators value, they won't be added either way.
    const withThousandsSeparators = true;
    assert.equal(
      expression.toLatex(numberOfSignificantDigits, withThousandsSeparators),
      '\\left(-1\\times 2+0.001\\right)^{π}',
    );

    // Test spaces after commas
    const expression2 = poincare.PCR_UserExpression.BuildFromPattern(
      'Add(Mult(MinusOne, One), Pi)',
    );
    assert.ok(!expression2.isUninitialized());
    assert.equal(
      expression2.toLatex(numberOfSignificantDigits, withThousandsSeparators),
      '-1\\times 1+π',
    );
  }),

  testCase('Solver - Min, Max, Root', (poincare) => {
    const emptyContext = new poincare.PCR_EmptyContext();
    const reductionContext = poincare.PCR_ReductionContext.Default(emptyContext, true);

    const systemFunction = poincare.PCR_UserExpression.BuildFromLatex('(x-4)(x+2)')
      .cloneAndReduce(reductionContext)
      .getSystemFunction('x');

    let solver;
    function resetSolver() {
      solver = new poincare.PCR_Solver(-10, 10, emptyContext);
      solver.stretch();
    }

    resetSolver();
    const firstRoot = solver.nextRoot(systemFunction);
    assert.equal(firstRoot.x(), -2);
    assert.equal(firstRoot.y(), 0);

    const secondRoot = solver.nextRoot(systemFunction);
    assert.equal(secondRoot.x(), 4);
    assert.equal(secondRoot.y(), 0);

    const thirdRoot = solver.nextRoot(systemFunction);
    assert.ok(Number.isNaN(thirdRoot.x()));
    assert.ok(Number.isNaN(thirdRoot.y()));

    resetSolver();
    const firstMin = solver.nextMinimum(systemFunction);
    assert.equal(firstMin.x(), 1);
    assert.equal(firstMin.y(), -9);

    const secondMin = solver.nextMinimum(systemFunction);
    assert.ok(Number.isNaN(secondMin.x()));
    assert.ok(Number.isNaN(secondMin.y()));

    resetSolver();
    const firstMax = solver.nextMaximum(systemFunction);
    assert.ok(Number.isNaN(firstMax.x()));
    assert.ok(Number.isNaN(firstMax.y()));
  }),
]).then(() => {
  console.log(
    '\n> All tests completed!\nTotal: ' +
      nTests +
      '\nSuccess: ' +
      nSuccess +
      '\nFails: ' +
      (nTests - nSuccess),
  );
  if (nSuccess < nTests) {
    process.exitCode = 1;
  }
});
