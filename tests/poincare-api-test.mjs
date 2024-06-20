import assert from 'node:assert/strict'
import Poincare from './poincare.mjs'

var Poincare = await Poincare()

class ArraySeries extends Poincare.Series.extend("PCR_Series", {}) {
  constructor(x, y) {
    super()
    if (x.length != y.length) {
      console.log("bad array")
    }
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
var regression = new Poincare.PCR_Regression(Poincare.RegressionType.Linear);
var coefficients = regression.fit(series);

assert.deepEqual(coefficients, [ 3.3995413996411177, -6.934805690848492, NaN, NaN, NaN ])

var prediction = regression.evaluate(coefficients, 10);

assert.equal(prediction, 27.06060830556268)
