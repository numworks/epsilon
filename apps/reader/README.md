# Thanks
Thanks to [Gabriel79](https://github.com/Gabriel79) for the original reader app, his source code available [here](https://github.com/Gabriel79/OmegaWithReaderTutorial) and the [tutorial](https://www.codingame.com/playgrounds/55846/reader-faire-une-application-pour-omega-sur-numworks/introduction) to code it !

---

# Rich text format
Reader app supports now a rich text format : 

 * `$` around a LaTeX expression to render it
 * `%` around a color-code (see below) to change the color of the text
### LaTeX expressions
You can read the documentation for the LaTeX Parser [here](TexParser.md).
### Color codes :
|code|color|
| --:| ---:|
|`%\last_color%`|Stop using last color|
|`%r%`|Red|
|`%rl%`|Light red|
|`%m%`|Magenta|
|`%t%`|Turquoise|
|`%pk%`|Pink|
|`%pp%`|Purple|
|`%b%`|Blue|
|`%bl%`|Light blue|
|`%br%`|Brown|
|`%o%`|Orange|
|`%g%`|Green|
|`%gl%`|Light green|
|`%c%`|Cyan|