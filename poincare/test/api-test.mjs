import assert from 'node:assert/strict'
import { CompilePoincareModuleFromFile, UsePoincare, UserExpressionTree } from './poincare.mjs'

console.log('\n> Initializing Poincare');

await CompilePoincareModuleFromFile('./poincare.wasm');

console.log('> Starting tests\n');

let nTests = 0;
let nSuccess = 0;

async function testCase(featureName, testFunction) {
  // Create a new Poincare instance
  UsePoincare(async (poincare) => {
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
  });
}

Promise.all([
  // Wait for all tests to complete before logging end message
  testCase('Regression - Linear', async (poincare) => {
    class ArraySeries extends poincare.PCR_Series.extend('PCR_Series', {}) {
      constructor(x, y) {
        super();
        assert.equal(x.length, y.length);
        this.x = x;
        this.y = y;
      }
      getX(i) {
        return this.x[i];
      }
      getY(i) {
        return this.y[i];
      }
      numberOfPairs() {
        return this.x.length;
      }
    }

    var series = new ArraySeries([1.0, 8.0, 14.0, 79.0], [-3.581, 20.296, 40.676, 261.623]);
    var regression = new poincare.PCR_Regression(poincare.RegressionType.LinearAxpb);
    var coefficients = regression.fit(series);

    assert.deepEqual(coefficients, [3.3995413996411177, -6.934805690848492, NaN, NaN, NaN]);

    var prediction = regression.evaluate(coefficients, 10);

    assert.equal(prediction, 27.06060830556268);
  }),

  testCase('Expression - Parse, Reduce, Approximate', async (poincare) => {
    const userExpression = poincare.PCR_UserExpression.BuildFromLatex('\\frac{6}{9}');

    assert.ok(!userExpression.isUninitialized());

    const emptyContext = new poincare.PCR_EmptyContext();
    const reductionContext = poincare.PCR_ReductionContext.Default(emptyContext, false);

    const reducedExpression = userExpression.cloneAndReduce(reductionContext);
    assert.ok(!reducedExpression.isUninitialized());

    const userReducedExpression = reducedExpression.cloneAndBeautify(reductionContext);
    assert.ok(!userReducedExpression.isUninitialized());
    assert.equal(userReducedExpression.toLatex(), '\\frac{2}{3}');

    const userApproximateExpression = reducedExpression
      .approximateToTree()
      .cloneAndBeautify(reductionContext);

    assert.ok(!userApproximateExpression.isUninitialized());
    assert.equal(userApproximateExpression.toLatex(), '0.6666667');
  }),

  testCase('Expression - System Function, Derivative', async (poincare) => {
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
    assert.equal(firstDerivative.toLatex(), 'dep\\left(2x-2,\\left(x^{2}\\right)\\right)');

    const secondDerivative = reducedExpression
      .getReducedDerivative('x', 2)
      .cloneAndBeautify(reductionContext);
    assert.ok(!secondDerivative.isUninitialized());
    assert.equal(secondDerivative.toLatex(), 'dep\\left(2,\\left(x^{2}\\right)\\right)');

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

    // Reinstantiate in a new Poincare instance
    UsePoincare((newPoincare) => {
      const newExpression = newPoincare.PCR_UserExpression.BuildFromTree(storedTree);
      assert.ok(!newExpression.isUninitialized());
      assert.equal(expression.toLatex(), '1+2');
    });
  }),

  testCase('Expression builder', async (poincare) => {
    const expression = poincare.PCR_UserExpression.BuildFromPattern(
      'Pow(Add(Mult(MinusOne,K0),K1),Pi)',
      poincare.PCR_UserExpression.BuildInt(2),
      poincare.PCR_UserExpression.BuildFloat(1e-3),
    );
    assert.ok(!expression.isUninitialized());
    assert.equal(expression.toLatex(), '\\left(-1\\times 2+0.001\\right)^{π}');

    // Test spaces after commas
    const expression2 = poincare.PCR_UserExpression.BuildFromPattern(
      'Add(Mult(MinusOne, One), Pi)'
    );
    assert.ok(!expression2.isUninitialized());
    assert.equal(expression2.toLatex(), '-1\\times 1+π');
  }),

  testCase('Solver - Min, Max, Root', async (poincare) => {
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
