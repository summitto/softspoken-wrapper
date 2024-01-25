const otModule = require('bindings')('ot-wrapper-node');

function arrayToLittleEndianBytes(inputArray) {
  const bytes = [];

  for (let i = 0; i < inputArray.length; i += 8) {
    let byte = 0;

    for (let j = 0; j < 8 && i + j < inputArray.length; j++) {
      const choice = inputArray[i + j];
      if (choice === 1 || choice === true) {
        byte |= (1 << j);
      }
    }

    bytes.push(byte);
  }

  return bytes;
}

/**
 * @param {ArrayLike<number> | ArrayLike<boolean>} arr A list of choices as numbers or booleans.
 * For a value at index i:
 * - if the ith value is 0 or false, then the 1st block in the ith pair is selected;
 * - if the ith value is 1 or true, then the 2nd block in the ith pair is selected.
 * @returns {[Uint8Array, number]} encoded choices as Uint8Array and the number of them
 */
function arrayToChoiceBits(arr) {
  if (!Array.isArray(arr)) {
    throw new Error('expected array');
  }

  let allValidNumbers = true;
  let allBooleans = true;

  for (const value of arr) {
    if (typeof value !== 'boolean') {
      allBooleans = false;
    }
    if (value !== 0 && value !== 1) {
      allValidNumbers = false;
    }

    // If both flags are false, we can break early.
    if (!allBooleans && !allValidNumbers) {
      break;
    }
  }

  if (!allValidNumbers && !allBooleans) {
    throw new Error('expected array of numbers (0 and 1) or booleans')
  }

  return [Uint8Array.from(arrayToLittleEndianBytes(Array.from(arr))), arr.length];
}

otModule.arrayToChoiceBits = arrayToChoiceBits;

module.exports = otModule; // reexport;
