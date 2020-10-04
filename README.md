# RegFetch
Interfaces with winsockets to fetch a txt file, parses the file and changes the registry accordingly

### Usage
In order to use this you'll have to adapt the URL and path in the main method function. 

currently only supports REG_EXPAND_SZ (harcoded), u can change it manually or create a STRUCT or an iterator to parse the data correctly. 
I myself didn't have the time yet to look into it, so other types will probably be supported in a feature release. 

The file format to set the registry key (as it is coded right now) is as follows:

```
HKEY_CURRENT_USER\Some\Path\Here
Name:demoname
Type:REG_EXPAND_SZ
Data:HiFromC
```

Feel free to implement encoders/encryption if you please, I will not support this in the public release. 

