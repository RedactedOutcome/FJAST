// Straight from Chat GPT
// Identifiers & Keywords
let myVar = 42;
const isTrue = true;
function add(a, b) {
    return a + b;
}

// StringLiteral & TemplateLiteral
let str = "Hello, world!";
let template = `Value: ${myVar}`;

// NumericalLiteral
let floatNum = 3.14;
let hexNum = 0xFF;
let expNum = 1e10;

// BoolLiteral & NullLiteral
let isFalse = false;
let nothing = null;

// Operators
let sum = 5 + 10 * 2;  // Arithmetic
let isEqual = (sum === 25);  // Conditional
let bitwise = sum & 0xFF;  // Bitwise
let logical = isTrue && isFalse;  // Logical
let result = isEqual ? "Yes" : "No";  // Ternary

// RegExpLiteral
let regex = /\d+/g;

// Grouping Symbols & Punctuators
if (isTrue) {
    console.log("This is true!");
} else {
    console.log("This is false!");
}

// Assignment Operator
myVar += 10;

// Function Call
console.log(add(2, 3));

// Object and Array
let obj = { key: "value", number: 100 };
let arr = [1, 2, 3, "test"];

// Loop
for (let i = 0; i < arr.length; i++) {
    console.log(arr[i]);
}

// Arrow Function
const multiply = (x, y) => x * y;

// Class (ES6 Feature)
class Person {
    constructor(name) {
        this.name = name;
    }
    greet() {
        console.log(`Hello, my name is ${this.name}`);
    }
}

let person = new Person("Alice");
person.greet();