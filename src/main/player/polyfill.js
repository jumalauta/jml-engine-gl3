// https://github.com/uxitten/polyfill
// Latest commit 53ded2a on Apr 13 2019

/*
MIT License

Copyright (c) 2019 Behnam Mohammadi

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// https://github.com/uxitten/polyfill/blob/master/array.polyfill.js

/**
 * Array.prototype.from()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    45      32      (No)                (Yes)   9       (Yes)
 * -------------------------------------------------------------------------------
 */
if (!Array.from) {
  Array.from = (function() {
    var toStr = Object.prototype.toString;
    var isCallable = function(fn) {
      return typeof fn === 'function' || toStr.call(fn) === '[object Function]';
    };
    var toInteger = function(value) {
      var number = Number(value);
      if (isNaN(number)) {
        return 0;
      }
      if (number === 0 || !isFinite(number)) {
        return number;
      }
      return (number > 0 ? 1 : -1) * Math.floor(Math.abs(number));
    };
    var maxSafeInteger = Math.pow(2, 53) - 1;
    var toLength = function(value) {
      var len = toInteger(value);
      return Math.min(Math.max(len, 0), maxSafeInteger);
    };

    // The length property of the from method is 1.
    return function from(arrayLike /*, mapFn, thisArg */) {
      // 1. Let C be the this value.
      var C = this;

      // 2. Let items be ToObject(arrayLike).
      var items = Object(arrayLike);

      // 3. ReturnIfAbrupt(items).
      if (arrayLike == null) {
        throw new TypeError(
          'Array.from requires an array-like object - not null or undefined'
        );
      }

      // 4. If mapfn is undefined, then let mapping be false.
      var mapFn = arguments.length > 1 ? arguments[1] : void undefined;
      var T;
      if (typeof mapFn !== 'undefined') {
        // 5. else
        // 5. a If IsCallable(mapfn) is false, throw a TypeError exception.
        if (!isCallable(mapFn)) {
          throw new TypeError(
            'Array.from: when provided, the second argument must be a function'
          );
        }

        // 5. b. If thisArg was supplied, let T be thisArg; else let T be undefined.
        if (arguments.length > 2) {
          T = arguments[2];
        }
      }

      // 10. Let lenValue be Get(items, "length").
      // 11. Let len be ToLength(lenValue).
      var len = toLength(items.length);

      // 13. If IsConstructor(C) is true, then
      // 13. a. Let A be the result of calling the [[Construct]] internal method
      // of C with an argument list containing the single item len.
      // 14. a. Else, Let A be ArrayCreate(len).
      var A = isCallable(C) ? Object(new C(len)) : new Array(len);

      // 16. Let k be 0.
      var k = 0;
      // 17. Repeat, while k < len… (also steps a - h)
      var kValue;
      while (k < len) {
        kValue = items[k];
        if (mapFn) {
          A[k] =
            typeof T === 'undefined'
              ? mapFn(kValue, k)
              : mapFn.call(T, kValue, k);
        } else {
          A[k] = kValue;
        }
        k += 1;
      }
      // 18. Let putStatus be Put(A, "length", len, true).
      A.length = len;
      // 20. Return A.
      return A;
    };
  })();
}

/**
 * Array.prototype.isArray()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support      5       4       9                   10.5  5       (Yes)
 * -------------------------------------------------------------------------------
 */
if (!Array.isArray) {
  Array.isArray = function(arg) {
    return Object.prototype.toString.call(arg) === '[object Array]';
  };
}

/**
 * Array.prototype.of()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support      45        25      (No)                  (No)  9       (Yes)
 * -------------------------------------------------------------------------------
 */
if (!Array.of) {
  Array.of = function() {
    return Array.prototype.slice.call(arguments);
  };
}

/**
 * Array.prototype.copyWithin()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support      45        32      (No)                  32    9       12
 * -------------------------------------------------------------------------------
 */
if (!Array.prototype.copyWithin) {
  Array.prototype.copyWithin = function(target, start /*, end*/) {
    // Steps 1-2.
    if (this == null) {
      throw new TypeError('this is null or not defined');
    }

    var O = Object(this);

    // Steps 3-5.
    var len = O.length >>> 0;

    // Steps 6-8.
    var relativeTarget = target >> 0;

    var to =
      relativeTarget < 0
        ? Math.max(len + relativeTarget, 0)
        : Math.min(relativeTarget, len);

    // Steps 9-11.
    var relativeStart = start >> 0;

    var from =
      relativeStart < 0
        ? Math.max(len + relativeStart, 0)
        : Math.min(relativeStart, len);

    // Steps 12-14.
    var end = arguments[2];
    var relativeEnd = end === undefined ? len : end >> 0;

    var final =
      relativeEnd < 0
        ? Math.max(len + relativeEnd, 0)
        : Math.min(relativeEnd, len);

    // Step 15.
    var count = Math.min(final - from, len - to);

    // Steps 16-17.
    var direction = 1;

    if (from < to && to < from + count) {
      direction = -1;
      from += count - 1;
      to += count - 1;
    }

    // Step 18.
    while (count > 0) {
      if (from in O) {
        O[to] = O[from];
      } else {
        delete O[to];
      }

      from += direction;
      to += direction;
      count--;
    }

    // Step 19.
    return O;
  };
}

/**
 * Array.prototype.entries()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support      38      28      (No)                25    7.1     ?
 * -------------------------------------------------------------------------------
 */

/**
 * Array.prototype.every()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support      (Yes)   1.5     9                  (Yes)  (Yes)   ?
 * -------------------------------------------------------------------------------
 */
if (!Array.prototype.every) {
  Array.prototype.every = function(callbackfn, thisArg) {
    'use strict';
    var T, k;

    if (this == null) {
      throw new TypeError('this is null or not defined');
    }

    // 1. Let O be the result of calling ToObject passing the this
    //    value as the argument.
    var O = Object(this);

    // 2. Let lenValue be the result of calling the Get internal method
    //    of O with the argument "length".
    // 3. Let len be ToUint32(lenValue).
    var len = O.length >>> 0;

    // 4. If IsCallable(callbackfn) is false, throw a TypeError exception.
    if (typeof callbackfn !== 'function') {
      throw new TypeError();
    }

    // 5. If thisArg was supplied, let T be thisArg; else let T be undefined.
    if (arguments.length > 1) {
      T = thisArg;
    }

    // 6. Let k be 0.
    k = 0;

    // 7. Repeat, while k < len
    while (k < len) {
      var kValue;

      // a. Let Pk be ToString(k).
      //   This is implicit for LHS operands of the in operator
      // b. Let kPresent be the result of calling the HasProperty internal
      //    method of O with argument Pk.
      //   This step can be combined with c
      // c. If kPresent is true, then
      if (k in O) {
        // i. Let kValue be the result of calling the Get internal method
        //    of O with argument Pk.
        kValue = O[k];

        // ii. Let testResult be the result of calling the Call internal method
        //     of callbackfn with T as the this value and argument list
        //     containing kValue, k, and O.
        var testResult = callbackfn.call(T, kValue, k, O);

        // iii. If ToBoolean(testResult) is false, return false.
        if (!testResult) {
          return false;
        }
      }
      k++;
    }
    return true;
  };
}

/**
 * Array.prototype.fill()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support      45      31      (No)                (No)  7.1     (Yes)
 * -------------------------------------------------------------------------------
 */
if (!Array.prototype.fill) {
  Object.defineProperty(Array.prototype, 'fill', {
    value: function(value) {
      // Steps 1-2.
      if (this == null) {
        throw new TypeError('this is null or not defined');
      }

      var O = Object(this);

      // Steps 3-5.
      var len = O.length >>> 0;

      // Steps 6-7.
      var start = arguments[1];
      var relativeStart = start >> 0;

      // Step 8.
      var k =
        relativeStart < 0
          ? Math.max(len + relativeStart, 0)
          : Math.min(relativeStart, len);

      // Steps 9-10.
      var end = arguments[2];
      var relativeEnd = end === undefined ? len : end >> 0;

      // Step 11.
      var final =
        relativeEnd < 0
          ? Math.max(len + relativeEnd, 0)
          : Math.min(relativeEnd, len);

      // Step 12.
      while (k < final) {
        O[k] = value;
        k++;
      }

      // Step 13.
      return O;
    }
  });
}

/**
 * Array.prototype.filter()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    (Yes)   1.5     9                    (Yes)  (Yes)   ?
 * -------------------------------------------------------------------------------
 */
if (!Array.prototype.filter) {
  Array.prototype.filter = function(fun /*, thisArg*/) {
    'use strict';

    if (this === void 0 || this === null) {
      throw new TypeError();
    }

    var t = Object(this);
    var len = t.length >>> 0;
    if (typeof fun !== 'function') {
      throw new TypeError();
    }

    var res = [];
    var thisArg = arguments.length >= 2 ? arguments[1] : void 0;
    for (var i = 0; i < len; i++) {
      if (i in t) {
        var val = t[i];

        // NOTE: Technically this should Object.defineProperty at
        //       the next index, as push can be affected by
        //       properties on Object.prototype and Array.prototype.
        //       But that method's new, and collisions should be
        //       rare, so use the more-compatible alternative.
        if (fun.call(thisArg, val, i, t)) {
          res.push(val);
        }
      }
    }

    return res;
  };
}

/**
 * Array.prototype.find()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support      45      25      (No)                  32    7.1     12
 * -------------------------------------------------------------------------------
 */
if (!Array.prototype.find) {
  Object.defineProperty(Array.prototype, 'find', {
    value: function(predicate) {
      // 1. Let O be ? ToObject(this value).
      if (this == null) {
        throw new TypeError('"this" is null or not defined');
      }

      var o = Object(this);

      // 2. Let len be ? ToLength(? Get(O, "length")).
      var len = o.length >>> 0;

      // 3. If IsCallable(predicate) is false, throw a TypeError exception.
      if (typeof predicate !== 'function') {
        throw new TypeError('predicate must be a function');
      }

      // 4. If thisArg was supplied, let T be thisArg; else let T be undefined.
      var thisArg = arguments[1];

      // 5. Let k be 0.
      var k = 0;

      // 6. Repeat, while k < len
      while (k < len) {
        // a. Let Pk be ! ToString(k).
        // b. Let kValue be ? Get(O, Pk).
        // c. Let testResult be ToBoolean(? Call(predicate, T, « kValue, k, O »)).
        // d. If testResult is true, return kValue.
        var kValue = o[k];
        if (predicate.call(thisArg, kValue, k, o)) {
          return kValue;
        }
        // e. Increase k by 1.
        k++;
      }

      // 7. Return undefined.
      return undefined;
    }
  });
}

/**
 * Array.prototype.findIndex()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support      45      25      (No)                  (Yes) 7.1     (Yes)
 * -------------------------------------------------------------------------------
 */
if (!Array.prototype.findIndex) {
  Object.defineProperty(Array.prototype, 'findIndex', {
    value: function(predicate) {
      // 1. Let O be ? ToObject(this value).
      if (this == null) {
        throw new TypeError('"this" is null or not defined');
      }

      var o = Object(this);

      // 2. Let len be ? ToLength(? Get(O, "length")).
      var len = o.length >>> 0;

      // 3. If IsCallable(predicate) is false, throw a TypeError exception.
      if (typeof predicate !== 'function') {
        throw new TypeError('predicate must be a function');
      }

      // 4. If thisArg was supplied, let T be thisArg; else let T be undefined.
      var thisArg = arguments[1];

      // 5. Let k be 0.
      var k = 0;

      // 6. Repeat, while k < len
      while (k < len) {
        // a. Let Pk be ! ToString(k).
        // b. Let kValue be ? Get(O, Pk).
        // c. Let testResult be ToBoolean(? Call(predicate, T, « kValue, k, O »)).
        // d. If testResult is true, return k.
        var kValue = o[k];
        if (predicate.call(thisArg, kValue, k, o)) {
          return k;
        }
        // e. Increase k by 1.
        k++;
      }

      // 7. Return -1.
      return -1;
    }
  });
}

/**
 * Array.prototype.flat()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support      69      62      (No)                56    12      (No)
 * -------------------------------------------------------------------------------
 */
if (!Array.prototype.flat) {
  Array.prototype.flat = function() {
    const stack = [].concat(this);
    const result = [];
    while (stack.length) {
      const next = stack.pop();
      if (Array.isArray(next)) stack.push.apply(stack, next);
      else result.push(next);
    }
    return result.reverse();
  };
}

/**
 * Array.prototype.flatMap()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support      69      62      (No)                56    12      (No)
 * -------------------------------------------------------------------------------
 */
if (!Array.prototype.flatMap) {
  Array.prototype.flatMap = function() {
    return Array.prototype.map.apply(this, arguments).flat(1);
  };
}

/**
 * Array.prototype.forEach()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support      (Yes)   1.5     9                   (Yes) (Yes)   ?
 * -------------------------------------------------------------------------------
 */
if (!Array.prototype.forEach) {
  Array.prototype.forEach = function(callback /*, thisArg*/) {
    var T, k;

    if (this == null) {
      throw new TypeError('this is null or not defined');
    }

    // 1. Let O be the result of calling toObject() passing the
    // |this| value as the argument.
    var O = Object(this);

    // 2. Let lenValue be the result of calling the Get() internal
    // method of O with the argument "length".
    // 3. Let len be toUint32(lenValue).
    var len = O.length >>> 0;

    // 4. If isCallable(callback) is false, throw a TypeError exception.
    // See: http://es5.github.com/#x9.11
    if (typeof callback !== 'function') {
      throw new TypeError(callback + ' is not a function');
    }

    // 5. If thisArg was supplied, let T be thisArg; else let
    // T be undefined.
    if (arguments.length > 1) {
      T = arguments[1];
    }

    // 6. Let k be 0
    k = 0;

    // 7. Repeat, while k < len
    while (k < len) {
      var kValue;

      // a. Let Pk be ToString(k).
      //    This is implicit for LHS operands of the in operator
      // b. Let kPresent be the result of calling the HasProperty
      //    internal method of O with argument Pk.
      //    This step can be combined with c
      // c. If kPresent is true, then
      if (k in O) {
        // i. Let kValue be the result of calling the Get internal
        // method of O with argument Pk.
        kValue = O[k];

        // ii. Call the Call internal method of callback with T as
        // the this value and argument list containing kValue, k, and O.
        callback.call(T, kValue, k, O);
      }
      // d. Increase k by 1.
      k++;
    }
    // 8. return undefined
  };
}

/**
 * Array.prototype.includes()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support      47      43      (No)                34    9       14
 * -------------------------------------------------------------------------------
 */
if (!Array.prototype.includes) {
  Object.defineProperty(Array.prototype, 'includes', {
    value: function(searchElement, fromIndex) {
      // 1. Let O be ? ToObject(this value).
      if (this == null) {
        throw new TypeError('"this" is null or not defined');
      }

      var o = Object(this);

      // 2. Let len be ? ToLength(? Get(O, "length")).
      var len = o.length >>> 0;

      // 3. If len is 0, return false.
      if (len === 0) {
        return false;
      }

      // 4. Let n be ? ToInteger(fromIndex).
      //    (If fromIndex is undefined, this step produces the value 0.)
      var n = fromIndex | 0;

      // 5. If n ≥ 0, then
      //  a. Let k be n.
      // 6. Else n < 0,
      //  a. Let k be len + n.
      //  b. If k < 0, let k be 0.
      var k = Math.max(n >= 0 ? n : len - Math.abs(n), 0);

      function sameValueZero(x, y) {
        return (
          x === y ||
          (typeof x === 'number' &&
            typeof y === 'number' &&
            isNaN(x) &&
            isNaN(y))
        );
      }

      // 7. Repeat, while k < len
      while (k < len) {
        // a. Let elementK be the result of ? Get(O, ! ToString(k)).
        // b. If SameValueZero(searchElement, elementK) is true, return true.
        // c. Increase k by 1.
        if (sameValueZero(o[k], searchElement)) {
          return true;
        }
        k++;
      }

      // 8. Return false
      return false;
    }
  });
}

/**
 * Array.prototype.indexOf()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support      (Yes)   1.5     9                   (Yes) (Yes)   ?
 * -------------------------------------------------------------------------------
 */
if (!Array.prototype.indexOf) {
  Array.prototype.indexOf = function(searchElement, fromIndex) {
    var k;

    // 1. Let o be the result of calling ToObject passing
    //    the this value as the argument.
    if (this == null) {
      throw new TypeError('"this" is null or not defined');
    }

    var o = Object(this);

    // 2. Let lenValue be the result of calling the Get
    //    internal method of o with the argument "length".
    // 3. Let len be ToUint32(lenValue).
    var len = o.length >>> 0;

    // 4. If len is 0, return -1.
    if (len === 0) {
      return -1;
    }

    // 5. If argument fromIndex was passed let n be
    //    ToInteger(fromIndex); else let n be 0.
    var n = fromIndex | 0;

    // 6. If n >= len, return -1.
    if (n >= len) {
      return -1;
    }

    // 7. If n >= 0, then Let k be n.
    // 8. Else, n<0, Let k be len - abs(n).
    //    If k is less than 0, then let k be 0.
    k = Math.max(n >= 0 ? n : len - Math.abs(n), 0);

    // 9. Repeat, while k < len
    while (k < len) {
      // a. Let Pk be ToString(k).
      //   This is implicit for LHS operands of the in operator
      // b. Let kPresent be the result of calling the
      //    HasProperty internal method of o with argument Pk.
      //   This step can be combined with c
      // c. If kPresent is true, then
      //    i.  Let elementK be the result of calling the Get
      //        internal method of o with the argument ToString(k).
      //   ii.  Let same be the result of applying the
      //        Strict Equality Comparison Algorithm to
      //        searchElement and elementK.
      //  iii.  If same is true, return k.
      if (k in o && o[k] === searchElement) {
        return k;
      }
      k++;
    }
    return -1;
  };
}

/**
 * Array.prototype.join()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    1       1       5.5                 (Yes) (Yes)   ?
 * -------------------------------------------------------------------------------
 */

/**
 * Array.prototype.keys()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support      38      28      (No)                25    7.1     (Yes)
 * -------------------------------------------------------------------------------
 */

/**
 * Array.prototype.lastIndexOf()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support      (Yes)   (Yes)   9                   (Yes) (Yes)   ?
 * -------------------------------------------------------------------------------
 */
if (!Array.prototype.lastIndexOf) {
  Array.prototype.lastIndexOf = function(searchElement /*, fromIndex*/) {
    'use strict';

    if (this === void 0 || this === null) {
      throw new TypeError();
    }

    var n,
      k,
      t = Object(this),
      len = t.length >>> 0;
    if (len === 0) {
      return -1;
    }

    n = len - 1;
    if (arguments.length > 1) {
      n = Number(arguments[1]);
      if (n != n) {
        n = 0;
      } else if (n != 0 && n != 1 / 0 && n != -(1 / 0)) {
        n = (n > 0 || -1) * Math.floor(Math.abs(n));
      }
    }

    for (k = n >= 0 ? Math.min(n, len - 1) : len - Math.abs(n); k >= 0; k--) {
      if (k in t && t[k] === searchElement) {
        return k;
      }
    }
    return -1;
  };
}

/**
 * Array.prototype.map()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    (Yes)   1.5     9                     (Yes) (Yes)   ?
 * -------------------------------------------------------------------------------
 */
if (!Array.prototype.map) {
  Array.prototype.map = function(callback /*, thisArg*/) {
    var T, A, k;

    if (this == null) {
      throw new TypeError('this is null or not defined');
    }

    // 1. Let O be the result of calling ToObject passing the |this|
    //    value as the argument.
    var O = Object(this);

    // 2. Let lenValue be the result of calling the Get internal
    //    method of O with the argument "length".
    // 3. Let len be ToUint32(lenValue).
    var len = O.length >>> 0;

    // 4. If IsCallable(callback) is false, throw a TypeError exception.
    // See: http://es5.github.com/#x9.11
    if (typeof callback !== 'function') {
      throw new TypeError(callback + ' is not a function');
    }

    // 5. If thisArg was supplied, let T be thisArg; else let T be undefined.
    if (arguments.length > 1) {
      T = arguments[1];
    }

    // 6. Let A be a new array created as if by the expression new Array(len)
    //    where Array is the standard built-in constructor with that name and
    //    len is the value of len.
    A = new Array(len);

    // 7. Let k be 0
    k = 0;

    // 8. Repeat, while k < len
    while (k < len) {
      var kValue, mappedValue;

      // a. Let Pk be ToString(k).
      //   This is implicit for LHS operands of the in operator
      // b. Let kPresent be the result of calling the HasProperty internal
      //    method of O with argument Pk.
      //   This step can be combined with c
      // c. If kPresent is true, then
      if (k in O) {
        // i. Let kValue be the result of calling the Get internal
        //    method of O with argument Pk.
        kValue = O[k];

        // ii. Let mappedValue be the result of calling the Call internal
        //     method of callback with T as the this value and argument
        //     list containing kValue, k, and O.
        mappedValue = callback.call(T, kValue, k, O);

        // iii. Call the DefineOwnProperty internal method of A with arguments
        // Pk, Property Descriptor
        // { Value: mappedValue,
        //   Writable: true,
        //   Enumerable: true,
        //   Configurable: true },
        // and false.

        // In browsers that support Object.defineProperty, use the following:
        // Object.defineProperty(A, k, {
        //   value: mappedValue,
        //   writable: true,
        //   enumerable: true,
        //   configurable: true
        // });

        // For best browser support, use the following:
        A[k] = mappedValue;
      }
      // d. Increase k by 1.
      k++;
    }

    // 9. return A
    return A;
  };
}

/**
 * Array.prototype.pop()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support      1       1       5.5                 (Yes) (Yes)   ?
 * -------------------------------------------------------------------------------
 */

/**
 * Array.prototype.push()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support      1       1       5.5                 (Yes) (Yes)   ?
 * -------------------------------------------------------------------------------
 */

/**
 * Array.prototype.reduce()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    (Yes)   3       9                     10.5  4       ?
 * -------------------------------------------------------------------------------
 */
if (!Array.prototype.reduce) {
  Object.defineProperty(Array.prototype, 'reduce', {
    value: function(callback /*, initialValue*/) {
      if (this === null) {
        throw new TypeError(
          'Array.prototype.reduce ' + 'called on null or undefined'
        );
      }
      if (typeof callback !== 'function') {
        throw new TypeError(callback + ' is not a function');
      }

      // 1. Let O be ? ToObject(this value).
      var o = Object(this);

      // 2. Let len be ? ToLength(? Get(O, "length")).
      var len = o.length >>> 0;

      // Steps 3, 4, 5, 6, 7
      var k = 0;
      var value;

      if (arguments.length >= 2) {
        value = arguments[1];
      } else {
        while (k < len && !(k in o)) {
          k++;
        }

        // 3. If len is 0 and initialValue is not present,
        //    throw a TypeError exception.
        if (k >= len) {
          throw new TypeError(
            'Reduce of empty array ' + 'with no initial value'
          );
        }
        value = o[k++];
      }

      // 8. Repeat, while k < len
      while (k < len) {
        // a. Let Pk be ! ToString(k).
        // b. Let kPresent be ? HasProperty(O, Pk).
        // c. If kPresent is true, then
        //    i.  Let kValue be ? Get(O, Pk).
        //    ii. Let accumulator be ? Call(
        //          callbackfn, undefined,
        //          « accumulator, kValue, k, O »).
        if (k in o) {
          value = callback(value, o[k], k, o);
        }

        // d. Increase k by 1.
        k++;
      }

      // 9. Return accumulator.
      return value;
    }
  });
}

/**
 * Array.prototype.reduceRight()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support      (Yes)   3       9                   10.5  4       ?
 * -------------------------------------------------------------------------------
 */
if ('function' !== typeof Array.prototype.reduceRight) {
  Array.prototype.reduceRight = function(callback /*, initialValue*/) {
    'use strict';
    if (null === this || 'undefined' === typeof this) {
      throw new TypeError('Array.prototype.reduce called on null or undefined');
    }
    if ('function' !== typeof callback) {
      throw new TypeError(callback + ' is not a function');
    }
    var t = Object(this),
      len = t.length >>> 0,
      k = len - 1,
      value;
    if (arguments.length >= 2) {
      value = arguments[1];
    } else {
      while (k >= 0 && !(k in t)) {
        k--;
      }
      if (k < 0) {
        throw new TypeError('Reduce of empty array with no initial value');
      }
      value = t[k--];
    }
    for (; k >= 0; k--) {
      if (k in t) {
        value = callback(value, t[k], k, t);
      }
    }
    return value;
  };
}

/**
 * Array.prototype.reverse()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support      1       1       5.5                   (Yes)   (Yes)   (Yes)
 * -------------------------------------------------------------------------------
 */

/**
 * Array.prototype.shift()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    1       1       5.5                 (Yes) (Yes)   (Yes)
 * -------------------------------------------------------------------------------
 */

/**
 * Array.prototype.slice()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support      1       1       5.5                   (Yes) (Yes)   (Yes)
 * -------------------------------------------------------------------------------
 */

/**
 * Array.prototype.some()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support      1       1.5     9                  (Yes)  (Yes)   ?
 * -------------------------------------------------------------------------------
 */
if (!Array.prototype.some) {
  Array.prototype.some = function(fun /*, thisArg*/) {
    'use strict';

    if (this == null) {
      throw new TypeError('Array.prototype.some called on null or undefined');
    }

    if (typeof fun !== 'function') {
      throw new TypeError();
    }

    var t = Object(this);
    var len = t.length >>> 0;

    var thisArg = arguments.length >= 2 ? arguments[1] : void 0;
    for (var i = 0; i < len; i++) {
      if (i in t && fun.call(thisArg, t[i], i, t)) {
        return true;
      }
    }

    return false;
  };
}

/**
 * Array.prototype.sort()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    1       1       5.5                 (Yes) (Yes)   (Yes)
 * -------------------------------------------------------------------------------
 */

/**
 * Array.prototype.splice()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    1       1       5.5                 (Yes) (Yes)   (Yes)
 * -------------------------------------------------------------------------------
 */

/**
 * Array.prototype.toLocaleString()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support      (Yes)   (Yes)   (Yes)                 (Yes) (Yes)   (Yes)
 * -------------------------------------------------------------------------------
 */
if (!Array.prototype.toLocaleString) {
  Object.defineProperty(Array.prototype, 'toLocaleString', {
    value: function(locales, options) {
      // 1. Let O be ? ToObject(this value).
      if (this == null) {
        throw new TypeError('"this" is null or not defined');
      }

      var a = Object(this);

      // 2. Let len be ? ToLength(? Get(A, "length")).
      var len = a.length >>> 0;

      // 3. Let separator be the String value for the
      //    list-separator String appropriate for the
      //    host environment's current locale (this is
      //    derived in an implementation-defined way).
      // NOTE: In this case, we will use a comma
      var separator = ',';

      // 4. If len is zero, return the empty String.
      if (len === 0) {
        return '';
      }

      // 5. Let firstElement be ? Get(A, "0").
      var firstElement = a[0];
      // 6. If firstElement is undefined or null, then
      //  a.Let R be the empty String.
      // 7. Else,
      //  a. Let R be ?
      //     ToString(?
      //       Invoke(
      //        firstElement,
      //        "toLocaleString",
      //        « locales, options »
      //       )
      //     )
      var r =
        firstElement == null
          ? ''
          : firstElement.toLocaleString(locales, options);

      // 8. Let k be 1.
      var k = 1;

      // 9. Repeat, while k < len
      while (k < len) {
        // a. Let S be a String value produced by
        //   concatenating R and separator.
        var s = r + separator;

        // b. Let nextElement be ? Get(A, ToString(k)).
        var nextElement = a[k];

        // c. If nextElement is undefined or null, then
        //   i. Let R be the empty String.
        // d. Else,
        //   i. Let R be ?
        //     ToString(?
        //       Invoke(
        //        nextElement,
        //        "toLocaleString",
        //        « locales, options »
        //       )
        //     )
        r =
          nextElement == null
            ? ''
            : nextElement.toLocaleString(locales, options);

        // e. Let R be a String value produced by
        //   concatenating S and R.
        r = s + r;

        // f. Increase k by 1.
        k++;
      }

      // 10. Return R.
      return r;
    }
  });
}

/**
 * Array.prototype.toString()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    (Yes)   (Yes)   (Yes)               (Yes) (Yes)   (Yes)
 * -------------------------------------------------------------------------------
 */

/**
 * Array.prototype.unshift()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support      1       1       5.5                 (Yes) (Yes)   ?
 * -------------------------------------------------------------------------------
 */

/**
 * Array.prototype.values()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    (No)    (No)    (No)                (No)  9       (Yes)
 * -------------------------------------------------------------------------------
 */

// https://github.com/uxitten/polyfill/blob/master/string.polyfill.js

/**
 * String.fromCharCode()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    (Yes)   (Yes)   (Yes)               (Yes)   (Yes)   (Yes)
 * -------------------------------------------------------------------------------
 */

/**
 * String.fromCodePoint()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    41      29      (No)                28      10      ?
 * -------------------------------------------------------------------------------
 */
if (!String.fromCodePoint) {
  (function() {
    var defineProperty = (function() {
      try {
        var object = {};
        var $defineProperty = Object.defineProperty;
        var result = $defineProperty(object, object, object) && $defineProperty;
      } catch (error) {}
      return result;
    })();
    var stringFromCharCode = String.fromCharCode;
    var floor = Math.floor;
    var fromCodePoint = function() {
      var MAX_SIZE = 0x4000;
      var codeUnits = [];
      var highSurrogate;
      var lowSurrogate;
      var index = -1;
      var length = arguments.length;
      if (!length) {
        return '';
      }
      var result = '';
      while (++index < length) {
        var codePoint = Number(arguments[index]);
        if (
          !isFinite(codePoint) ||
          codePoint < 0 ||
          codePoint > 0x10ffff ||
          floor(codePoint) != codePoint
        ) {
          throw RangeError('Invalid code point: ' + codePoint);
        }
        if (codePoint <= 0xffff) {
          // BMP code point
          codeUnits.push(codePoint);
        } else {
          codePoint -= 0x10000;
          highSurrogate = (codePoint >> 10) + 0xd800;
          lowSurrogate = (codePoint % 0x400) + 0xdc00;
          codeUnits.push(highSurrogate, lowSurrogate);
        }
        if (index + 1 == length || codeUnits.length > MAX_SIZE) {
          result += stringFromCharCode.apply(null, codeUnits);
          codeUnits.length = 0;
        }
      }
      return result;
    };
    if (defineProperty) {
      defineProperty(String, 'fromCodePoint', {
        value: fromCodePoint,
        configurable: true,
        writable: true
      });
    } else {
      String.fromCodePoint = fromCodePoint;
    }
  })();
}

/**
 * String.anchor()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    (Yes)   1.0     (Yes)               (Yes)   (Yes)   (Yes)
 * -------------------------------------------------------------------------------
 */

/**
 * String.charAt()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    (Yes)   (Yes)   (Yes)               (Yes)   (Yes)   (Yes)
 * -------------------------------------------------------------------------------
 */

/**
 * String.charCodeAt()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    (Yes)   1.0     (Yes)               (Yes)   (Yes)   (Yes)
 * -------------------------------------------------------------------------------
 */

/**
 * String.codePointAt()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    41      29      11                  28      10      ?
 * -------------------------------------------------------------------------------
 */
if (!String.prototype.codePointAt) {
  (function() {
    'use strict';
    var codePointAt = function(position) {
      if (this == null) {
        throw TypeError();
      }
      var string = String(this);
      var size = string.length;
      var index = position ? Number(position) : 0;
      if (index != index) {
        index = 0;
      }
      if (index < 0 || index >= size) {
        return undefined;
      }
      var first = string.charCodeAt(index);
      var second;
      if (first >= 0xd800 && first <= 0xdbff && size > index + 1) {
        second = string.charCodeAt(index + 1);
        if (second >= 0xdc00 && second <= 0xdfff) {
          return (first - 0xd800) * 0x400 + second - 0xdc00 + 0x10000;
        }
      }
      return first;
    };
    if (Object.defineProperty) {
      Object.defineProperty(String.prototype, 'codePointAt', {
        value: codePointAt,
        configurable: true,
        writable: true
      });
    } else {
      String.prototype.codePointAt = codePointAt;
    }
  })();
}

/**
 * String.concat()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    (Yes)   (Yes)   (Yes)               (Yes)   (Yes)   (Yes)
 * -------------------------------------------------------------------------------
 */

/**
 * String.endsWith()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    41      17      (No)                (No)    9       (Yes)
 * -------------------------------------------------------------------------------
 */
if (!String.prototype.endsWith) {
  String.prototype.endsWith = function(searchString, position) {
    var subjectString = this.toString();
    if (
      typeof position !== 'number' ||
      !isFinite(position) ||
      Math.floor(position) !== position ||
      position > subjectString.length
    ) {
      position = subjectString.length;
    }
    position -= searchString.length;
    var lastIndex = subjectString.lastIndexOf(searchString, position);
    return lastIndex !== -1 && lastIndex === position;
  };
}

/**
 * String.includes()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    41      40      (No)                (No)    9       (Yes)
 * -------------------------------------------------------------------------------
 */
if (!String.prototype.includes) {
  String.prototype.includes = function(search, start) {
    'use strict';
    if (typeof start !== 'number') {
      start = 0;
    }
    if (start + search.length > this.length) {
      return false;
    } else {
      return this.indexOf(search, start) !== -1;
    }
  };
}

/**
 * String.indexOf()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    (Yes)   (Yes)   (Yes)               (Yes)   (Yes)   (Yes)
 * -------------------------------------------------------------------------------
 */

/**
 * String.lastIndexOf()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    (Yes)   (Yes)   (Yes)               (Yes)   (Yes)   (Yes)
 * -------------------------------------------------------------------------------
 */

/**
 * String.link()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    (Yes)   1.0    (Yes)                (Yes)   (Yes)   (Yes)
 * -------------------------------------------------------------------------------
 */

/**
 * String.localeCompare()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    (Yes)   1.0    (Yes)                (Yes)   (Yes)   (Yes)
 * -------------------------------------------------------------------------------
 */

/**
 * String.match()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    (Yes)   (Yes)   (Yes)               (Yes)   (Yes)   (Yes)
 * -------------------------------------------------------------------------------
 */

/**
 * String.normalize()
 * version 0.0.1
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    34      31      (No)                (Yes)   10      (Yes)
 * -------------------------------------------------------------------------------
 */
if (!String.prototype.normalize) {
  // need polyfill
}

/**
 * String.padEnd()
 * version 1.0.1
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    57      48      (No)                44      10      15
 * -------------------------------------------------------------------------------
 */
if (!String.prototype.padEnd) {
  String.prototype.padEnd = function padEnd(targetLength, padString) {
    targetLength = targetLength >> 0; //floor if number or convert non-number to 0;
    padString = String(typeof padString !== 'undefined' ? padString : ' ');
    if (this.length > targetLength) {
      return String(this);
    } else {
      targetLength = targetLength - this.length;
      if (targetLength > padString.length) {
        padString += padString.repeat(targetLength / padString.length); //append to original to ensure we are longer than needed
      }
      return String(this) + padString.slice(0, targetLength);
    }
  };
}

/**
 * String.padStart()
 * version 1.0.1
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    57      51      (No)                44      10      15
 * -------------------------------------------------------------------------------
 */
if (!String.prototype.padStart) {
  String.prototype.padStart = function padStart(targetLength, padString) {
    targetLength = targetLength >> 0; //floor if number or convert non-number to 0;
    padString = String(typeof padString !== 'undefined' ? padString : ' ');
    if (this.length > targetLength) {
      return String(this);
    } else {
      targetLength = targetLength - this.length;
      if (targetLength > padString.length) {
        padString += padString.repeat(targetLength / padString.length); //append to original to ensure we are longer than needed
      }
      return padString.slice(0, targetLength) + String(this);
    }
  };
}

/**
 * String.repeat()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    41      24      (No)                (Yes)   9       (Yes)
 * -------------------------------------------------------------------------------
 */
if (!String.prototype.repeat) {
  String.prototype.repeat = function(count) {
    'use strict';
    if (this == null) {
      throw new TypeError("can't convert " + this + ' to object');
    }
    var str = '' + this;
    count = +count;
    if (count != count) {
      count = 0;
    }
    if (count < 0) {
      throw new RangeError('repeat count must be non-negative');
    }
    if (count == Infinity) {
      throw new RangeError('repeat count must be less than infinity');
    }
    count = Math.floor(count);
    if (str.length == 0 || count == 0) {
      return '';
    }
    if (str.length * count >= 1 << 28) {
      throw new RangeError(
        'repeat count must not overflow maximum string size'
      );
    }
    var rpt = '';
    for (;;) {
      if ((count & 1) == 1) {
        rpt += str;
      }
      count >>>= 1;
      if (count == 0) {
        break;
      }
      str += str;
    }
    return rpt;
  };
}

/**
 * String.search()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    (Yes)   (Yes)   (Yes)               (Yes)   (Yes)   (Yes)
 * -------------------------------------------------------------------------------
 */

/**
 * String.slice()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    (Yes)   (Yes)   (Yes)               (Yes)   (Yes)   (Yes)
 * -------------------------------------------------------------------------------
 */

/**
 * String.split()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    (Yes)   (Yes)   (Yes)               (Yes)   (Yes)   (Yes)
 * -------------------------------------------------------------------------------
 */

/**
 * String.startsWith()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    41      17      (No)                28      9       (Yes)
 * -------------------------------------------------------------------------------
 */
if (!String.prototype.startsWith) {
  String.prototype.startsWith = function(searchString, position) {
    position = position || 0;
    return this.substr(position, searchString.length) === searchString;
  };
}

/**
 * String.substr()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    (Yes)   (Yes)   (Yes)               (Yes)   (Yes)   (Yes)
 * -------------------------------------------------------------------------------
 */

/**
 * String.substring()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    (Yes)   (Yes)   (Yes)               (Yes)   (Yes)   (Yes)
 * -------------------------------------------------------------------------------
 */

/**
 * String.toLocaleLowerCase()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    (Yes)   (Yes)   (Yes)               (Yes)   (Yes)   (Yes)
 * -------------------------------------------------------------------------------
 */

/**
 * String.toLocaleUpperCase()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    (Yes)   (Yes)   (Yes)               (Yes)   (Yes)   (Yes)
 * -------------------------------------------------------------------------------
 */

/**
 * String.toLowerCase()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    (Yes)   (Yes)   (Yes)               (Yes)   (Yes)   (Yes)
 * -------------------------------------------------------------------------------
 */

/**
 * String.toString()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    (Yes)   (Yes)   (Yes)               (Yes)   (Yes)   (Yes)
 * -------------------------------------------------------------------------------
 */

/**
 * String.toUpperCase()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    (Yes)   (Yes)   (Yes)               (Yes)   (Yes)   (Yes)
 * -------------------------------------------------------------------------------
 */

/**
 * String.trim()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    (Yes)   3.5     9                   10.5    5       ?
 * -------------------------------------------------------------------------------
 */
if (!String.prototype.trim) {
  String.prototype.trim = function() {
    return this.replace(/^[\s\uFEFF\xA0]+|[\s\uFEFF\xA0]+$/g, '');
  };
}

/**
 * String.trimLeft()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    (Yes)   3.5     (No)                ?       ?       ?
 * -------------------------------------------------------------------------------
 */

/**
 * String.trimRight()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    (Yes)   3.5     (No)                ?       ?       ?
 * -------------------------------------------------------------------------------
 */

/**
 * String.valueOf()
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    (Yes)   (Yes)   (Yes)               (Yes)   (Yes)   (Yes)
 * -------------------------------------------------------------------------------
 */

/**
 * String.raw
 * version 0.0.0
 * Feature          Chrome  Firefox Internet Explorer   Opera   Safari  Edge
 * Basic support    41      34      (No)                (No)    10      ?
 * -------------------------------------------------------------------------------
 */
