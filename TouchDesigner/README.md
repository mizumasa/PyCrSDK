# PyCrSDK for Touch Desigher

# How to Install

1. Open Terminal as ADMINISTRATOR  

2. Use the python instance that TouchDesigner has internally to install the PyCrSDK using the pip command.  

You need to change the command path according to the folder TouchDesigner is installed (ex.. C:\Program Files\Derivative\TouchDesigner)

```
>>> & "C:\Program Files\Derivative\TouchDesigner\bin\python.exe" -m pip install --force-reinstall --no-deps --target "C:\Program Files\Derivative\TouchDesigner\bin\Lib\site-packages" pycrsdk-0.1.0-py3-none-any.whl

Processing c:\masaru\crsdk\pycrsdk\installer\dist\pycrsdk-0.1.0-py3-none-any.whl
Installing collected packages: pycrsdk
Successfully installed pycrsdk-0.1.0

>>> & "C:\Program Files\Derivative\TouchDesigner\bin\python.exe" -c "import PyCrSDK"

Installed DLLs to C:\Program Files\Derivative\TouchDesigner\bin\CrAdapter

```


3. Please check there is a "PyCrSDK" folder in the "C:\Program Files\Derivative\TouchDesigner\bin\Lib\site-packages"
   
<img width="859" height="633" alt="image" src="https://github.com/user-attachments/assets/20c1d7d9-7fbb-4556-a93d-65479cf9389b" />
  
  
4. Please check there is a "CrAdapter" folder in the "C:\Program Files\Derivative\TouchDesigner\bin"

<img width="872" height="426" alt="image" src="https://github.com/user-attachments/assets/a0f82f28-8a59-4e82-86a8-2144b98d9ce9" />
  
  
5. Check PyCrSDK is installed

<img width="734" height="350" alt="image" src="https://github.com/user-attachments/assets/e9d07716-09c8-4c25-8762-28773cdae1fc" />
 
