import util from 'util'

import { Int } from './types/types'
import { isFloat, roundToPlaces } from './utils'

function makeString(n: number) {
    const ans = n.toString()

    if (ans.indexOf('.') === -1) return ans + '.0'

    return ans
}

export class Fraction {
    numerator!: Int
    denominator!: Int

    constructor(fraction: Fraction, _?: any)
    constructor(numerator: Int, denominator: Int)
    constructor(numerator: Int | Fraction, denominator?: Int) {
        if (numerator instanceof Fraction) {
            this.numerator = numerator.numerator
            this.denominator = numerator.denominator
        } else {
            this.numerator = numerator
            this.denominator = denominator!
        }

        this.normalize()
    }
    normalize() {
        const _normalizeScale = (toRound: Int, toScale: Int) => {
            const scaleup = Math.pow(10, makeString(roundToPlaces(toRound, 9)).split('.')[1].length)
            return [Math.round(toRound * scaleup), toScale * scaleup]
        }

        if (isFloat(this.denominator))
            [this.numerator, this.denominator] = _normalizeScale(this.denominator, this.numerator)

        if (isFloat(this.numerator))
            [this.numerator, this.denominator] = _normalizeScale(this.numerator, this.denominator)

        let [gcf, b] = [Math.abs(this.numerator), Math.abs(this.denominator)]

        while (b) [gcf, b] = [b, gcf % b]

        this.numerator /= gcf
        this.denominator /= gcf

        if (this.denominator < 0) {
            this.numerator *= -1
            this.denominator *= -1
        }

        return this
    }

    clone = () => new Fraction(this.numerator, this.denominator)

    toString() {
        if (isNaN(this.denominator)) return 'NaN'

        let result = ''

        if (this.numerator < 0 != this.denominator < 0) result = '-'

        let numerator = Math.abs(this.numerator)

        const denominator = Math.abs(this.denominator)

        const wholepart = Math.floor(numerator / denominator)

        if (wholepart != 0) result += wholepart
        if ((numerator = numerator % denominator) != 0) {
            if (wholepart != 0) result += ' '
            result += numerator + '/' + denominator
        }

        return result.length > 0 ? result : '0'
    }

    [util.inspect.custom] = () => this.toString()

    rescale(factor: Int) {
        this.numerator *= factor
        this.denominator *= factor
        return this
    }

    add(b: Fraction) {
        const a = this.clone()
        if (b instanceof Fraction) {
            b = b.clone()
        } else {
            b = new Fraction(b)
        }
        const td = a.denominator
        a.rescale(b.denominator)
        a.numerator += b.numerator * td

        return a.normalize()
    }

    subtract(b: Fraction) {
        const a = this.clone()
        if (b instanceof Fraction) {
            b = b.clone()
        } else {
            b = new Fraction(b)
        }
        const td = a.denominator
        a.rescale(b.denominator)
        a.numerator -= b.numerator * td

        return a.normalize()
    }

    multiply(b: Int | Fraction): Fraction {
        const a = this.clone()
        if (b instanceof Fraction) {
            a.numerator *= b.numerator
            a.denominator *= b.denominator
        } else if (typeof b === 'number') {
            a.numerator *= b
        }

        return a.normalize()
    }

    divide(b: Int | Fraction): Fraction {
        const a = this.clone()
        if (b instanceof Fraction) {
            a.numerator *= b.denominator
            a.denominator *= b.numerator
        } else if (typeof b === 'number') {
            a.denominator *= b
        } else {
            return a.divide(new Fraction(b))
        }
        return a.normalize()
    }
}
