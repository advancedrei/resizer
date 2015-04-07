﻿// This file is licensed CC0, and can be considered 'insignificant'
// Apply whichever license you like
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System;
using ImageResizer.Util;
using System.Reflection;

// General Information about an assembly is controlled through the following 
// set of attributes. Change these attribute values to modify the information
// associated with an assembly.

#if TRIAL
[assembly: BuildType("trial")]
[assembly: AssemblyProduct("Image Resizer Plugin - requires license key")]
#else
[assembly: AssemblyProduct("Image Resizer")]
#endif

[assembly: AssemblyCompany("Imazen LLC")]
[assembly: AssemblyCopyright("Copyright © 2015 Imazen LLC")]

// Version information for an assembly consists of the following four values:
//
//      Major Version
//      Minor Version 
//      Build Number
//      Revision
//
// You can specify all the values or you can default the Revision and Build Numbers 
// by using the '*' as shown below:/*
[assembly: AssemblyVersion("4.0.0.0")]
[assembly: AssemblyFileVersion("4.0.0.591")]
[assembly: AssemblyInformationalVersion("4.0.0-prerelease.591")]

[assembly: Commit("331d72ae9881981a0cce986083afee8b934f6eae")]



// These commented out settings are for the build script to access
// [assembly: PackageName("Resizer")]
// [assembly: NugetVersion("4.0.0-prerelease")]
// [assembly: DownloadServer("http://downloads.imageresizing.net/")]
