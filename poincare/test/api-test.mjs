import assert from 'node:assert/strict'
import Poincare from './poincare.mjs'
import fs from 'fs'

console.log('> Initializing Poincare');

var wasmFile = fs.readFileSync('./poincare.wasm');
const wasmBinary = new Uint8Array(wasmFile);

console.log('> Starting tests\n');

let nTests = 0;
let nSuccess = 0;

async function testCase(featureName, testFunction) {
  // Create a new Poincare instance
  const poincare = await Poincare({ wasmBinary: wasmBinary });
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
  console.log((success ? '✅ '  : '❌ ') + "Test: " + featureName);
  if (!success) {
    console.log(error);
  }
}

Promise.all([ // Wait for all tests to complete before logging end message
  testCase("Regression - Linear", async (poincare) => {
    class ArraySeries extends poincare.PCR_Series.extend("PCR_Series", {}) {
      constructor(x, y) {
        super()
        assert.equal(x.length, y.length)
        this.x = x
        this.y = y
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

    var series = new ArraySeries([1.0, 8.0, 14.0, 79.0],
                                [-3.581, 20.296, 40.676, 261.623]);
    var regression = new poincare.PCR_Regression(poincare.RegressionType.LinearAxpb);
    var coefficients = regression.fit(series);

    assert.deepEqual(coefficients, [ 3.3995413996411177, -6.934805690848492, NaN, NaN, NaN ])

    var prediction = regression.evaluate(coefficients, 10);

    assert.equal(prediction, 27.06060830556268);
  }),

  testCase("Expression - Parse, Reduce, Approximate", async (poincare) => {
    const userExpression = poincare.PCR_Expression.ParseLatex("\\frac{6}{9}");

    assert.ok(!userExpression.isUninitialized());

    const emptyContext = new poincare.PCR_EmptyContext();
    const reductionContext = new poincare.PCR_ReductionContext(
      emptyContext,
      poincare.ComplexFormat.Cartesian,
      poincare.AngleUnit.Radian,
      poincare.UnitFormat.Metric,
      poincare.ReductionTarget.SystemForAnalysis,
      poincare.SymbolicComputation.ReplaceAllDefinedSymbolsWithDefinition,
      poincare.UnitConversion.Default,
    );

    const reducedExpression = userExpression.cloneAndReduce(reductionContext);
    assert.ok(!reducedExpression.isUninitialized());

    const userReducedExpression = reducedExpression.cloneAndBeautify(reductionContext);
    assert.ok(!userReducedExpression.isUninitialized());
    assert.equal(userReducedExpression.toLatex(), "\\frac{2}{3}");

    const approximationContext =
    poincare.PCR_ApproximationContext.FromReductionContext(reductionContext);
    const userApproximateExpression = reducedExpression
      .approximateToTree(approximationContext)
      .cloneAndBeautify(reductionContext);

    assert.ok(!userApproximateExpression.isUninitialized());
    assert.equal(userApproximateExpression.toLatex(), "0.6666667");
  }),

  testCase("Expression - System Function, Derivative", async (poincare) => {
    const userExpression = poincare.PCR_Expression.ParseLatex("x^{2}-2x+1");

    assert.ok(!userExpression.isUninitialized());

    const emptyContext = new poincare.PCR_EmptyContext();
    const reductionContext = new poincare.PCR_ReductionContext(
      emptyContext,
      poincare.ComplexFormat.Cartesian,
      poincare.AngleUnit.Radian,
      poincare.UnitFormat.Metric,
      poincare.ReductionTarget.SystemForAnalysis,
      poincare.SymbolicComputation.ReplaceAllDefinedSymbolsWithDefinition,
      poincare.UnitConversion.Default,
    );

    const reducedExpression = userExpression.cloneAndReduce(reductionContext);
    assert.ok(!reducedExpression.isUninitialized());

    const systemFunction = reducedExpression.getSystemFunction('x');
    assert.ok(!systemFunction.isUninitialized());
    assert.equal(systemFunction.approximateToScalarWithValue(3), 4);

    const firstDerivative = reducedExpression.getReducedDerivative('x', 1).cloneAndBeautify(reductionContext);
    assert.ok(!firstDerivative.isUninitialized());
    assert.equal(firstDerivative.toLatex(), "dep\\left(2x-2,\\left(x^{2}\\right)\\right)");

    const secondDerivative = reducedExpression.getReducedDerivative('x', 2).cloneAndBeautify(reductionContext);
    assert.ok(!secondDerivative.isUninitialized());
    assert.equal(secondDerivative.toLatex(), "dep\\left(2,\\left(x^{2}\\right)\\right)");
  }),

  testCase("Expression - Retrieve tree from CPP heap", async (poincare) => {
    const expression = poincare.PCR_Expression.ParseLatex("1+2");
    assert.ok(!expression.isUninitialized());
    const storedTree = expression.tree().toUint8Array();
    const expectedTree = new Uint8Array([19, 2, 6, 7]);
    assert.deepEqual(storedTree, expectedTree);

    // Reinstantiate in a new Poincare instance
    const newPoincare = await Poincare({ wasmBinary: wasmBinary });

    const newExpression = newPoincare.PCR_Expression.Builder(
      newPoincare.PCR_Tree.FromUint8Array(storedTree),
    );
    assert.ok(!newExpression.isUninitialized());
    assert.equal(expression.toLatex(), "1+2");
  }),

  testCase("Solver - Min, Max, Root", async (poincare) => {
    const emptyContext = new poincare.PCR_EmptyContext();
    const reductionContext = new poincare.PCR_ReductionContext(
      emptyContext,
      poincare.ComplexFormat.Cartesian,
      poincare.AngleUnit.Radian,
      poincare.UnitFormat.Metric,
      poincare.ReductionTarget.SystemForAnalysis,
      poincare.SymbolicComputation.ReplaceAllDefinedSymbolsWithDefinition,
      poincare.UnitConversion.Default,
    );

    const systemFunctionTree =
      poincare.PCR_Expression
        .ParseLatex("(x-4)(x+2)")
        .cloneAndReduce(reductionContext)
        .getSystemFunction('x').tree();

    let solver;
    function resetSolver() {
      solver = new poincare.PCR_Solver(-10, 10, emptyContext);
      solver.stretch();
    }

    resetSolver();
    const firstRoot = solver.nextRoot(systemFunctionTree);
    assert.equal(firstRoot.x(), -2);
    assert.equal(firstRoot.y(), 0);

    const secondRoot = solver.nextRoot(systemFunctionTree);
    assert.equal(secondRoot.x(), 4);
    assert.equal(secondRoot.y(), 0);

    const thirdRoot = solver.nextRoot(systemFunctionTree);
    assert.ok(Number.isNaN(thirdRoot.x()));
    assert.ok(Number.isNaN(thirdRoot.y()));

    resetSolver();
    const firstMin = solver.nextMinimum(systemFunctionTree);
    assert.equal(firstMin.x(), 1);
    assert.equal(firstMin.y(), -9);

    const secondMin = solver.nextMinimum(systemFunctionTree);
    assert.ok(Number.isNaN(secondMin.x()));
    assert.ok(Number.isNaN(secondMin.y()));

    resetSolver();
    const firstMax = solver.nextMaximum(systemFunctionTree);
    assert.ok(Number.isNaN(firstMax.x()));
    assert.ok(Number.isNaN(firstMax.y()));
  }),
])
.then(() => {
  console.log('\n> All tests completed!\nTotal: ' + nTests + '\nSuccess: ' + nSuccess + '\nFails: ' + (nTests - nSuccess));
  if (nSuccess < nTests) {
    process.exitCode = 1;
  }
});
