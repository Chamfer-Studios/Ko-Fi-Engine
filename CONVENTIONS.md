# Code Style C++ Guidelines

## Just-a-Glance Guideline

This table lists some main coding conventions rules the team will follow when developing the project.<br>
More specific questions and extended explanations will be written below.

Code element | Convention | Example
--- | :---: | ---
Macros | ALL_CAPS | `#define MIN(a,b) (((a)<(b))?(a):(b))`
Defines | ALL_CAPS | `#define PLATFORM_DESKTOP`
Constants | lowerCase | `const int maxValue = 8`
Struct | TitleCase | `struct Texture2D`
Struct members |lowerCase | `texture.id`
Enum | TitleCase | `TextureFormat`
Enum members | ALL_CAPS | `UNCOMPRESSED_R8G8B8`
Functions | TitleCase or prefixTitleCase | `InitWindow()`
Variables | lowerCase | `screenWidth`
Local variables | lowerCase | `playerPosition`
Global variables | lowerCase | `fullscreen`
Operators | value1*value2 | `int product = value*6;`
Operators | value1/value2 | `int division = value/4;`
Operators | value1 + value2 | `int sum = value + 10;`
Operators | value1 - value2 | `int res = value - 5;`
Pointers | MyType* pointer | `Texture2D* array;`
float values | always x.xf | `float value = 10.0f`
Ternary Operator | (condition)? result1 : result2 | `printf("Value is 0: %s", (value == 0)? "yes" : "no");`

The curly braces need to be in Aliman style.
```c
int main(int argc, char *argv[])
{
    ...
    while (x == y) 
    {
        something();
        somethingelse();

        if (some_error)
            do_correct(); // In K&R a single-statement block does not have braces.
        else
            continue_as_usual();
    }

    finalthing();
    ...
}
```
When proposing new functions, please try to use a clear naming for function-name and functions-parameters, in case of doubt, open an issue for discussion.

## 1. Naming Conventions

- All names should be written in English.
- Use Get/Set to get or set variables, do not use synonyms.
- Names must never have dual meaning, to ensure all concepts are represented uniquely.


**Variables**: must be in camel case, starting with lower case.

- Single name: 
```
speed
```
- Compound name:
```
cameraShake
```
- Temporal variables (only used in a few lines of code) should have short names, whilst long-term variables (used multiple times along the code) can have long names.
- Temporal variable:
```
m
```
- Long-term variable:
```
cameraShake
```
- Generic variables: should have the same name as their type.
```
void SetState(PlayerState* playerState)
```

**Enums**: Must be all uppercase using underscore to separate words and in singular.
```
PLAYER_STATE_MACHINE
```
- Enums items: must start with the name of the enum, followed by the name of the item. The name of the item must be in uppercase.
```
STATE_IDLE
```
```
STATE_TURN_RIGHT
```
- If the name of the enum is long and composed, the elements can start with a shortening of the enum, followed by the name of the item. The name of the item must be in uppercase.
```
PSM_IDLE
```
```
PSM_TURN_RIGHT
```

- When creating a variable of the type enum, call it as the enum.
```
PLAYER_STATE playerState;
```

- When referring to an enum item, simply write its name.
```
playerState = PLAYER_STATE_IDLE`
```

**Constants**: Must be all uppercase using underscore to separate words.
- Also applies to define variables

```
MOVEMENT_SPEED
```

**Functions and methods**: 
- Must be a short explanation of what the function or method does and must be written in mixed cade starting with uppercase
```
LoadMap()
```
- Keep in mind the object the function or method is referred to is implicit and should be avoided in the function or method.
```
player.GetState();
```

- Names representing a collection of numbers must be in plural
```
int values[];
```

- Bool variables should use the is prefix
```
bool isSet, isVisible, isOpen;
```

File content must be kept within 160 columns. If you need more columns split the code onto 2 or more lines. Keep in easy to read.
- You can break:
  - After a comma
  - After an operator
Align the new line with the beginning of the expression on the previous line.

```
for (int table = 0; table < nTables;
     table += tableStep) {
```
```
if ((c1.type == COLLIDER_ARROW && c2.type == COLLIDER_IMP) ||
    (c1.type == COLLIDER_ARROW && c2.type == COLLIDER_IMP))
```



**Includes**:
- Should be grouped by their hierarchical position in the system with low levels files first. Leave an empty line between groups of include statements.
```
#include <iostream>

#include "SDL\include\SDL_rect.h"

#include “map.h”
#include “player.h”
```


**Iterator variables**:
- If the variable is a number, use i, (and j, k for nested loops):

```
for (int i = 0; i < points; ++i)
```
```
for (int j = 0; j < points; ++j)
```


- Else, use the standard naming convention:
```
for (pugi::node animationsNode = node.child(“animation”); animationsNode;
     animationsNode.next_sibling(“animation”))
```
**Number variables**:
- If iterators, increase/decrease them using ++variable/--variable.
```
++i
```

- Else, increase/decrease them using variable++/variable--.
```  
speed++
```

**Whitespace**:
- Use space to improve readability. Add space between operators and operands, commas, and semicolons, and colons. Add space on the outside of parentheses and brackets .
```
a = (b + c) * d
```
```
while (true)
```
```
doSomething(a, b, c, d)
```
```
for (i = 0; i < 10; i++)
```

- Same applies to functions/methods and initialization of variables
```
App->render->DrawQuad({ 1000, 10, 3, 13 }, 255, 255, 255, 255);
```
```
SDL_Rect rect{ 1, 1, 1, 1 }
```
```
GetProperty("Draw", false)
```

**Parentheses**:
- In compound arithmetic expressions add parentheses, even if they don’t change order of operations.
```
a = (b * c) + (d / e) - (f % g)
```
- In Boolean expressions you should add parentheses around sub-expressions.
```
if ((x + (2 * y) > z) || (p < q))
```

**Between Lines**:
- Add one blank line between each logical block of code.
```
Matrix4x4 matrix = new Matrix4x4();

double cosAngle = Math.cos(angle);
double sinAngle = Math.sin(angle);

matrix.setElement(1, 1, cosAngle);
matrix.setElement(1, 2, sinAngle);
matrix.setElement(2, 1, -sinAngle);
matrix.setElement(2, 2, cosAngle);

multiply(matrix);
```

**Comments**:

- Put a descriptive comment before a logical block of code. Use // comments rather than	/* ... */ comments.
```
// do X
...
```

- In variables comments use // in the same line.
```
int player //Player x coordinate
```

## 2. Variables

- Never use magic numbers!
```
speed = MAX_SPEED
```
```
speed = maxSpeed
```
- If there is more than one variable of the same type, declarate them in the same line.
```
int i, j, k, l
```

- After its declaration, always initialize them.
	- Pointers must be initialized with default nullptr (over NULL).
```
char* name = nullptr
```
- Floating numbers must be initialized with default 0.0f.
```
float speed = 0.0f
```
- Integer and unsigned integer numbers must be initialized with default 0.
```
int childs = 0
```

- Booleans must be initialized with true or false.

```
bool isChild = true
```

- Minimize the use of global variables.

**Pointers and references**:
- should have their reference symbol next to the type rather than to the name.
```
char* name = nullptr`
```
**Classes**:
- It is preferred to use private (or protected) variables and access functions instead of public variables, which violate the encapsulation concept (hiding information).

**Structs**:
- It is preferred to use global variables.

## 3. Loops

- do-while loops can be avoided.
- Only use break and continue if they give higher readability.

**Infinite loops**: 
- Use the while(true) form

```
while (true)
```

## 4. Conditionals

- If: when using ifs avoid the use of operators whenever possible.
```
if (isOpen) {}
```
```
if (!isOpen) {}
```

- The conditional should be put on a separate line.
```
if (isOpen)
	Close()
```

- When working with booleans, put their true form in the if-part and leave the negative one for the else-part.
```
if (isOpen)
	Close()
else
	Open()
```

- If there is only one sentence after the if-else statement, don’t use brackets.
```
if (isOpen)
	Close()
else
	Open()
```
```
if (isHit)
{
	speed--;
	ApplySpeed();
}

if (isHit) {}
else if (!isHit) {}
```

## 5. Classes and Structs
Use a struct when the class is essentially a data structure.

- Class elements must be written in this order
```
Class SomeClass
{
public:
	constructors and destructors

	public functions and methods

private:
	private functions and methods

public:
	public variables

private: 
        private variables
};
```

## 6. XML

- Booleans: must be 0 or 1.

<fullscreen value=“0”/>
<fullscreen value=“false”/>

- XML data structure

Animations:
```
<animations>
	<idle speed=“10” loop=“1”>
		<frame x=“1” y=“454” w=“48” h=“48”/>
		<frame x=“50” y=“454” w=“48” h=“48”/>
	</idle>
</animations>
```

Map types:
```
<map type="0">
	<rooms>
		<room type="0" y="0" x="0">
			<doors>
				<door direction="0"/>
				<door direction="1"/>
				<door direction="2"/>
			</doors>
		</room>

		<room type="1" y="0" x="0">
			<doors>
				<door direction="0"/>
			</doors>
		</room>

		<room type="0" y="0" x="0">
			<doors>
				<door direction="0"/>
				<door direction="1"/>
				<door direction="2"/>
				<door direction="3"/>
			</doors>
		</room>
	</rooms>
</map>

```
 ## 8. JSON
 For the JSON files we will format them in the following convention:
 - Format JSON files to be human readable.
 - Use a tab for indentation or 4 spaces.
 - Use one space after the name-separator (:)
 - Follow the formal JSON conventinos. Wrap strings in double (not single) quotes. Numbers and other data types without quotes.
 - Sample files may have their keys ordered if that makes the file easier to understand. Automatic reformatting tools preserve the order of keys.
 ```
{
  "Engine": {
    "Authors": "Alejandro Avila & Bosco Barber",
    "Organization": "Ko-Fi Studios",
    "Title": "Ko-Fi Engine",
    "MaxFPS": 60
  },
  "Window": {
    "Width": 1024,
    "Height": 768,
    "Scale": 1,
    "Fullscreen": false,
    "FullscreenDesktop": false,
    "Resizable": true,
    "Borderless": false,
    "Title": "Ko-Fi Engine",
    "Icon": "Assets/Icons/icon.bmp"
  },
  "Renderer3D": {
    "Vsync": true
  },
  "Camera": null,
  "Input": null,
  "Editor": null,
  "SceneManager": null,
  "ModelLoader": null,
  "ViewportFrameBuffer": null
}

```
 
 ## 9. Files and Directories Naming Conventions
 
  - Directories are named using `TitleCase` : `Assets/Audio/Music`
  
  - Files are named using `snake_case`: `main_title.png`
  
  _NOTE: Spaces and special characters are avoided in the files/dir naming_
  
