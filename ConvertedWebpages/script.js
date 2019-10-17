/* script.js : defines toggleDisplay functionality for
 *             project 2 - OOD S2019
 * Author : Ammar Salman 
*/

var hiddenF = false;
var hiddenC = false;
var hiddenc = false;

function toggleVisibility(classname){
	var elems = document.getElementsByClassName(classname);
	var i;
	
	if(classname === "function"){
		hiddenF = !hiddenF;
		for (i = 0; i < elems.length; i++) { 
		  elems[i].hidden = hiddenF;
		}
		
	}
	else if(classname === "class"){
		hiddenC = !hiddenC;
		for (i = 0; i < elems.length; i++) { 
		  elems[i].hidden = hiddenC;
		}
	}
	else if(classname === "comment"){
		hiddenc = !hiddenc;
		for (i = 0; i < elems.length; i++) { 
		  elems[i].hidden = hiddenc;
		}
	}
}