'use strict';
//require('../../common');
var assert = require('assert');
var rundll = require('../build/Release/rundll');
console.log(rundll.LoadLibrary("Kernel32.dll"));
rundll.printf("hello rundll%d\n",2333);
