---
title: Debugging Code Templates
nav_order: 3
has_children: true
---
# Debugging Code Templates
{: no_toc}
A coding template provides example code that utilizes the Arduino Debugger to help find the bug(s) in your code.  The codint templates are broken up into 3 categories - _Conditional Logic_, _Loops_, and _Methods_.

1. TOC
{:toc}


<style>
.indent_25{
  margin-left:25px;
}

.indent_50{
  margin-left:50px;
}
  
</style>

## Loops

### Key Breakpoints
{: no_toc}
{: .indent_25}
When working with any type of loop, there are four key areas you'll want to place your breakpoints:
{: .indent_50}
<ol class="indent_50">
  <li><span style="font-weight:bold">Before</span> the loop
   <p>This breakpoint will help you see the current state of your variables & pins BEFORE any part of your loop is executed.</p>
  </li>
  <li><span style="font-weight:bold">Last line</span> of your loop 
   <p>This breakpoint helps to check that your ENDING your loop with correct values.</p>
  </li>
</ol>

### Bug Tips & Things to Check
{: no_toc}
{: .indent_25}
<dl class="indent_50">
   <dt>Counter Update</dt>
   <dd>Is your counter updating by the correct amount? Try writing out all of the values your counter should reach; is your code reaching all of those values?</dd>
   <dt>End Condition</dt>
   <dd>Is your loop ending on the correct value? A common mistake here is using "<" or ">" when you ended to use "<=" or ">=". For example, if you want your loop to count from 0 to 10, then the last value of the counter should be 11.<br>
   </dd>
 </dl>
 
### For Loop Logic
{: .no_toc}
{: .indent_25}
```cpp
  byte i = 0; //initialize for loop counter
  debugger.add(&i, BYTE, "i");//add loop counter to variable watch
  //Add other variables to watch here
  debugger.breakpoint("Before For Loop");
  for(i; i < end_condition; i++){
    //Your code here
    debugger.breakpoint("End For Loop");
  }
```
{: .indent_50}

### While Loop Logic
{: .no_toc}
{: .indent_25}
```cpp
  byte i = 0; //initialize for loop counter
  debugger.add(&i, BYTE, "i");//add loop counter to variable watch
  //Add other variables to watch here
  debugger.breakpoint("Before While Loop");
  while(i < end_condition)
    //Your code here
    i++;
    debugger.breakpoint("End While Loop");
  }
```
{: .indent_50}

### Do While Loop Logic
{: .no_toc}
{: .indent_25}
```cpp
  byte i = 0; //initialize for loop counter
  debugger.add(&i, BYTE, "i");//add loop counter to variable watch
  //Add other variables to watch here
  debugger.breakpoint("Before Do While Loop");
  do{
    //Your code here
    i++;
    debugger.breakpoint("End Do While Loop");
  }while(i < end_condition);
```
{: .indent_50}
