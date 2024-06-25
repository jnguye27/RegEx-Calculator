# RegEx
Description:  
Finds if there is a match within the regular expression given from the file input for it.  
  
To compile:  
make  
  
To run:  
./regex  
  
To note:  
* Files should have a first line (regex) and a second line (text).  
* If there are any newlines after the second line within the file, it may output an extra index for matchStar().  
* matchStar() is giving too many matches for its type when there is a single char.  
