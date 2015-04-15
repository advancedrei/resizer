Tag: install

# Troubleshooting guide

ImageResizer is self-diagnosing for the majority of issues. Visit `/resizer.debug.ashx` on your website to check for any issues, and to verify that you are using the latest version of all the ImageResizer dlls. If `/resizer.debug.ashx` is not available, you can check the version of the dlls by right-clicking on each and choosing "Properties."

To get support or use this guide, [make sure you get the detailed error message from visiting the image URL directly](/docs/geterror). This guide cannot offer a solution to a generic 500 error, 404 error, or a "broken image icon", as those symptoms are far to generic to be useful. 

If this page doesn't resolve your issue, [visit the Support page](/support) for information about the bug bounty and free support requirements.


##Server Error in '/' Application. Out of memory.

You might be trying to resize certain large images for the first time. Even a 15MB jpg, however, uncompresses to about 80MB in bitmap form (depending on the compression level). If you are resizing to a 2MB jpg (15MB BMP), memory requirements for the operation are roughly 110MB (15 + 80 + 15). If you plan on using the resizer for very high-resolution photos (above 8MP), we suggest making sure you have ample amounts of RAM. 400MB to 1GB is usually plenty for the average web site with disk caching enabled.

## Error 5 Missing compiler required member 'System.Runtime.CompilerServices.ExtensionAttribute..ctor'

Please update to V3.2.2 or later; Extension methods were temporarily introduced in V3.2.0 but [support proved untenable](http://stackoverflow.com/a/10996336/166893), and they will be delayed until V4.

## Could not load type 'ImageResizer.InterceptModule'

Potential causes:

1. Your website has  a 'sub-site' (Application Folder) inside it. Application Folders inherit all Web.config settings from their parent sites, yet expect to have their own copies of all the dlls referenced by those settings in their own /bin folder. You can resolve this problem by (a) changing the app folder to a virtual folder, (b) adding a copy of ImageResizer.dll and plugins into the /bin folder inside that application also, or (c) using `<remove />` statements in the child Web.config to cancel out the inherited `<add />` statements from the parent Web.config. Option (c) will disable image resizing within the sub-application.
2. You didn't copy ImageResizer.dll into the /bin folder.
3. The ImageResizer.dll file in /bin is corrupt. Verify it has the correct file size and version number, or re-copy it from the download. 

## Blank page, invalid image error in browser, or Page cannot be displayed on image URL.

If this happens when the image has a querystring:

* You are using ASP.NET MVC, but do not have the [MvcRoutingShim](/plugins/mvcroutingshim) plugin enabled. Both MVC and the ImageResizer are trying to answer the request, and corrupting the response.

If this only happens when the image doesn't have a querystring, you're running IIS6, and you have mapped individual extensions to ASP.NET instead of wildcard mapping:

* See [this fantastic Microsoft KB article for ways to fix this](http://support.microsoft.com/Default.aspx?kbid=909641).

Keep reading if it's a `404: File Not Found error`


## The type or namespace name "ImageResizer" could not be found

When using the ImageResizer from a .NET project that is not a web project you may get the following build error:

> The type or namespace name "ImageResizer" could not be found (are you missing a using directive or assembly reference?)

This is caused by using a Client Profile version of .NET instead of the Full version. You can change this in Project Properties -> Application -> Target Framework. The ImageResizer requires the full version, as it is also designed to support ASP.NET usage and references the System.Web assembly (which is not part of the client profile version of .NET).


## File not found

Potential causes:

1. You are using the .jpg.ashx syntax, and you did not register the HttpModule properly in both places of your Web.config file.
2. You are using the .jpg.ashx syntax, but you're not using a query string. You should drop the '.ashx' unless you actually want to process the file.
3. You are using ASP.NET MVC, but do not have the [MvcRoutingShim plugin](/plugins/mvcroutingshim) installed.
4. You have Precompilation enabled, and are using an image provider. [This is caused by a long-standing bug in the .NET framework](http://stackoverflow.com/questions/12397/net-virtualpathproviders-and-pre-compilation).

## File not found for JSON requests (Faces, RedEye plugins)

1. You have not copied Newtonsoft.Json.dll into the /bin folder, and are using .NET 4.0.

## Image appears original size

Potential causes: 

1. You did not register the HttpModule properly in both places of your Web.config file.
2. You are using IIS 6 (or earlier), or IIS7 Classic Pipeline, and are not using the .jpg.ashx syntax, and you have not [mapped all requests to the ASP.NET runtime](/docs/cleanurls). 
3. You are using ASP.NET MVC (and have conflicting routes), but do not have the [MvcRoutingShim plugin](/plugins/mvcroutingshim) installed.
4. You are mistyping the querystring commands. 
5. The original image is smaller than the size you are requesting, and you are not using `&scale=both` (The default behavior is to never upscale images, [but this can be changed](/plugins/defaultsettings)).

## File not found only on images without a querystring

Possible causes

1. You are using the .ashx syntax. This is normal - you should only add the .ashx suffix if you are also adding a querystring to tell the image resizer to do something.

2. The images are from S3 or Sql: The S3Reader and SqlReader plugins require AspNetHostingPermission to register themselves as VirtualPathProviders. If they lack permission, they register as IVirtualImageProviders instead - but then only the image resizer can find them, and the image resizer only accepts requests with a querystring that uses one of the defined commands.

3. Alternatively, you may have a URL rewriting event that is affecting the path names.


## This type of page is not served. (HTTP 403 error)

  Description: The type of page you have requested is not served because it has been explicitly forbidden.  The extension '.jpg' may be incorrect.   Please review the URL below and make sure that it is spelled correctly. 

Possible causes

1. You aren't logged in. ImageResizer obeys your URL Authorization rules, so don't expect to view images where you can't visit .aspx pages. 
2. You are trying to access a S3 bucket or Remote URL that is not authorized.

This generic error message often hides a more descriptive message, but that message is always guaranteed to be a "Not Authorized To View this Content" kind of error.



## Method Not Found (System.MissingMethodException)

  Server Error in '/' Application. 
  ------------------------------ 
  Method not found: 'Void System.Web.HttpContext.RemapHandler(System.Web.IHttpHandler)'.
  Exception Details: System.MissingMethodException: Method not found: 'Void 
  System.Web.HttpContext.RemapHandler(System.Web.IHttpHandler)'.
  
This error occurs when you do not have .NET 2.0 Service Pack 2 applied. No other conditions cause this error. The RemapHanlder method was added in .NET 2.0 SP2, and is required by this product.
Yes, it is possible to have .NET 3, 3.5, and .NET 4 installed (and up-to-date), yet not have .NET SP2 installed. This peculiar circumstance has been verified on over 5 machines.



## Server object error 'ASP 0178 : 80070005'

  Server object error 'ASP 0178 : 80070005'
  Server.CreateObject Access Error
  The call to Server.CreateObject failed while checking permissions. 
  Access is denied to this object.

This error usually means that the user that the ASP website is running under does not have NTFS permissions to the ImageResizer dlls. 
Right click the `C:\Program Files\ImageResizingNet\v3` folder and choose Properties, Security, hit Edit, then click Add, type in the user name your website is running under, hit OK, then check "Read & Execute", and hit OK, then Apply.

On IIS6, this [account is typically IUSR_ComputerName](http://www.microsoft.com/technet/prodtechnol/WindowsServer2003/Library/IIS/3648346f-e4f5-474b-86c7-5a86e85fa1ff.mspx?mfr=true), but on IIS7, the account is usually `NETWORK SERVICE` or (if you're not use a default app pool), a custom user account. You'll need to open IIS and inspect the appropriate Application Pool to find out which account you need to give permissions to. 

If that fails, providing Readonly access to the Everyone group should work, but that may not be acceptable if you have highly-isolated application pools which you don't want to be able to access the ImageResizer dlls files.

If you still encounter issues, perform a reinstall with COMInstaller.exe, and save the install log. If the reinstall doesn't fix the problem, send the install log to support@imageresizing.net to get help with your issue.

## System.Security.SecurityException

  System.Security.SecurityException: Request for the 
  permission of type 'System.Configuration.ConfigurationPermission, 
  System.Configuration, Version=2.0.0.0, Culture=neutral, 
  PublicKeyToken=b03f5f7f11d50a3a' failed

This can occur in certain medium or low trust environments. To correct, set `requirePermission="false"` on the resizer section declaration in Web.config.

  <?xml version="1.0" encoding="utf-8" ?>
  <configuration>
    <configSections>
      <section name="resizer" type="ImageResizer.ResizerSection,ImageResizer" requirePermission="false" />
    </configSections>
    ...

## Error parsing Web.Config

If you are using NuGet and recently upraded from an older release, there may be duplicate XML elements in Web.Config. Delete the one lacking the requirePermission="false" attribute.

  <section name="resizer" type="ImageResizer.ResizerSection"/>
  <section name="resizer" type="ImageResizer.ResizerSection" requirePermission="false"/>

## Quality loss when resizing 8-bit Grayscale Jpeg images

This is a known bug in GDI+. GDI+ opens 8-bit grayscale Jpeg images as 4-bit images. Here's the [bug report at Microsoft Connect](http://connect.microsoft.com/VisualStudio/feedback/details/649605/gdi-and-wic-opening-an-8bpp-grayscale-jpeg-results-in-quality-loss).

The workaround is to use WIC or FreeImage for these requests. Any of the following plugins will solve the problem

1. WicDecoder - Install, then add `&decoder=wic` to affected URLs. (best quality)
2. WicBuilder - Install, then add `&builder=wic` to affected URLs. Faster, but slightly lower quality than #1 (nearly imperceptible)
3. FreeImageDecoder - Install, then add `&decoder=freeimage`
4. FreeImageBuilder - Install, then add `&builder=freeimage`. Slowest, but highest quality.
 with `builder=freeimage` or `builder=wic`

## Performance issues or error messages when using a SAN.

This is tricky to get right - [read the full article here](http://imageresizing.net/docs/howto/avoid-network-limit).

## Losing transparency when working with GIF images

You must have the [PrettyGifs](/plugins/prettygifs) plugin installed to get high-quality, transparent GIF and 8-bit PNG results. You may also want the [AnimatedGifs](/plugins/animatedgifs) plugin.

## SizeLimitException - The dimensions of the output image (4800x2700) exceed the maximum permitted dimensions of 3200x3200.

By default, ImageResizer limits the output size of images to 3200x3200. This can be changed [by configuring (or removing) the SizeLimiting plugin](/plugins/sizelimiting).

## 0-byte response with status 200 returned when DiskCache is enabled

IIS 7+ gives you granular control over which IIS components can be installed. If the Static Content module is not installed, you will get empty responses for disk-cached content. 

If you have enabled asyncWrites, the first request per URL will succeed, but all subsequent operations will fail.

You need at minimum the following IIS modules installed for correct operation:

* Static Content
* HTTP Errors
* HTTP Redirection
* URL Authorization
* .NET Extensibility 4.5
* ASP.NET 4.5
