# 7z FFI SDK - Documentation Index

Welcome to the 7z FFI SDK documentation! This index will help you find the information you need.

## 📚 Quick Navigation

### Getting Started
- **[README.md](README.md)** - Project overview, features, and quick introduction
- **[QUICKSTART.md](QUICKSTART.md)** - Step-by-step guide to get up and running
- **[SETUP.md](SETUP.md)** - Detailed installation and build instructions
- **[demo.sh](demo.sh)** - Executable demo script showing the library in action

### Development
- **[TODO.md](TODO.md)** - Current tasks, roadmap, and feature tracking
- **[ROADMAP.md](ROADMAP.md)** - Long-term vision and release timeline
- **[CONTRIBUTING.md](CONTRIBUTING.md)** - Guidelines for contributing to the project
- **[PROJECT_SUMMARY.md](PROJECT_SUMMARY.md)** - Complete project status and summary

### API Documentation
- **[include/7z_ffi.h](include/7z_ffi.h)** - Complete C API documentation with all functions

### Integration Guides
- **[tauri/tauri-integration.md](tauri/tauri-integration.md)** - Complete guide for Tauri integration with Rust FFI

## 🎯 By Use Case

### "I want to use this library"
1. Start with **[QUICKSTART.md](QUICKSTART.md)**
2. Check **[README.md](README.md)** for overview
3. Run **[demo.sh](demo.sh)** to see it working
4. Reference **[include/7z_ffi.h](include/7z_ffi.h)** for API details

### "I want to integrate with Tauri"
1. Read **[tauri/tauri-integration.md](tauri/tauri-integration.md)**
2. Check **[include/7z_ffi.h](include/7z_ffi.h)** for API
3. See examples in **examples/** directory

### "I want to contribute"
1. Read **[CONTRIBUTING.md](CONTRIBUTING.md)** for guidelines
2. Check **[TODO.md](TODO.md)** for available tasks
3. Review **[ROADMAP.md](ROADMAP.md)** for future plans
4. Look at **[PROJECT_SUMMARY.md](PROJECT_SUMMARY.md)** for current status

### "I want to build from source"
1. Follow **[SETUP.md](SETUP.md)** for prerequisites
2. Run `./setup_lzma.sh` to download LZMA SDK
3. Use standard CMake build process
4. Run **[demo.sh](demo.sh)** to verify

### "I need help troubleshooting"
1. Check **[QUICKSTART.md](QUICKSTART.md)** troubleshooting section
2. Review **[SETUP.md](SETUP.md)** for setup issues
3. See **[TODO.md](TODO.md)** for known issues
4. Create an issue on GitHub

## 📖 Documentation Structure

### Overview Documents
| Document | Purpose | Audience |
|----------|---------|----------|
| README.md | Project introduction and features | Everyone |
| PROJECT_SUMMARY.md | Comprehensive project status | Maintainers, Contributors |
| QUICKSTART.md | Fast track to using the library | New users |

### Technical Documents
| Document | Purpose | Audience |
|----------|---------|----------|
| include/7z_ffi.h | Complete API reference | Developers |
| SETUP.md | Build and installation | Developers |
| tauri/tauri-integration.md | Tauri integration guide | Tauri developers |

### Planning Documents
| Document | Purpose | Audience |
|----------|---------|----------|
| TODO.md | Task list and current work | Contributors |
| ROADMAP.md | Future plans and timeline | Everyone |
| CONTRIBUTING.md | Contribution guidelines | Contributors |

### Executable Resources
| File | Purpose | Usage |
|------|---------|-------|
| setup_lzma.sh | Download LZMA SDK | `./setup_lzma.sh` |
| demo.sh | Interactive demo | `./demo.sh` |
| examples/* | Example programs | Build and run |

## 🔍 Find Information By Topic

### Installation & Setup
- Prerequisites: [SETUP.md](SETUP.md#prerequisites)
- LZMA SDK setup: [SETUP.md](SETUP.md#download-lzma-sdk)
- Build instructions: [QUICKSTART.md](QUICKSTART.md#installation)
- Platform notes: [QUICKSTART.md](QUICKSTART.md#platform-specific-notes)

### API Usage
- Initialization: [include/7z_ffi.h](include/7z_ffi.h) → `sevenzip_init()`
- Extract archive: [include/7z_ffi.h](include/7z_ffi.h) → `sevenzip_extract()`
- List contents: [include/7z_ffi.h](include/7z_ffi.h) → `sevenzip_list()`
- Error handling: [include/7z_ffi.h](include/7z_ffi.h) → Error codes section

### Examples
- List archive: [examples/list.c](examples/list.c)
- Extract archive: [examples/extract.c](examples/extract.c)
- Compression: [examples/compress.c](examples/compress.c) (placeholder)

### Integration
- Tauri: [tauri/tauri-integration.md](tauri/tauri-integration.md)
- Rust FFI: [tauri/tauri-integration.md](tauri/tauri-integration.md#step-2-create-a-rust-ffi-wrapper)
- C/C++ projects: [QUICKSTART.md](QUICKSTART.md#api-overview)

### Development
- Feature requests: [TODO.md](TODO.md#planned-features)
- Current status: [PROJECT_SUMMARY.md](PROJECT_SUMMARY.md)
- Future plans: [ROADMAP.md](ROADMAP.md)
- How to contribute: [CONTRIBUTING.md](CONTRIBUTING.md)

### Testing
- Test archive: `test_archive.7z`
- Demo script: [demo.sh](demo.sh)
- Test results: [PROJECT_SUMMARY.md](PROJECT_SUMMARY.md#test-results)

## 🎓 Learning Path

### Beginner
1. **[README.md](README.md)** - Understand what the project does
2. **[QUICKSTART.md](QUICKSTART.md)** - Build and run your first example
3. **[demo.sh](demo.sh)** - See it in action
4. **[examples/](examples/)** - Study the example code

### Intermediate
1. **[include/7z_ffi.h](include/7z_ffi.h)** - Learn the complete API
2. **[tauri/tauri-integration.md](tauri/tauri-integration.md)** - Integrate with your app
3. **[PROJECT_SUMMARY.md](PROJECT_SUMMARY.md)** - Understand architecture
4. Create your own application using the library

### Advanced
1. **[CONTRIBUTING.md](CONTRIBUTING.md)** - Learn contribution guidelines
2. **[TODO.md](TODO.md)** - See what needs work
3. **[src/](src/)** - Study the implementation
4. **[ROADMAP.md](ROADMAP.md)** - Understand future direction
5. Contribute code or features

## 📊 Document Status

| Document | Status | Last Updated | Completeness |
|----------|--------|--------------|--------------|
| README.md | ✅ Complete | 2026-01-27 | 100% |
| QUICKSTART.md | ✅ Complete | 2026-01-27 | 100% |
| SETUP.md | ✅ Complete | 2026-01-27 | 100% |
| PROJECT_SUMMARY.md | ✅ Complete | 2026-01-27 | 100% |
| TODO.md | ✅ Complete | 2026-01-27 | 100% |
| ROADMAP.md | ✅ Complete | 2026-01-27 | 100% |
| CONTRIBUTING.md | ✅ Complete | 2026-01-27 | 100% |
| tauri-integration.md | ✅ Complete | 2026-01-27 | 100% |
| 7z_ffi.h | ✅ Complete | 2026-01-27 | 100% |

## 🔗 External Resources

### LZMA SDK
- Official: https://www.7-zip.org/sdk.html
- Documentation: Included in LZMA SDK download

### Tauri
- Official: https://tauri.app/
- Documentation: https://tauri.app/v1/guides/

### CMake
- Official: https://cmake.org/
- Documentation: https://cmake.org/documentation/

## 📱 Quick Reference Card

```
Build Commands:
  ./setup_lzma.sh           # Download LZMA SDK
  mkdir build && cd build   # Create build directory
  cmake ..                  # Configure
  cmake --build .           # Build

Test Commands:
  ./demo.sh                 # Run demo
  ./build/examples/example_list archive.7z
  ./build/examples/example_extract archive.7z output/

Key Files:
  include/7z_ffi.h          # API header
  build/lib7z_ffi.dylib     # Built library
  examples/                 # Example code

Documentation:
  README.md                 # Start here
  QUICKSTART.md            # Quick tutorial
  tauri/tauri-integration.md # Tauri guide
```

## 💡 Tips

- **New to the project?** Start with README.md and QUICKSTART.md
- **Building from source?** Follow SETUP.md step by step
- **Integrating with Tauri?** Go directly to tauri/tauri-integration.md
- **Want to contribute?** Read CONTRIBUTING.md and check TODO.md
- **Stuck on something?** Check PROJECT_SUMMARY.md for detailed info

## 🤝 Getting Help

1. **Read the docs** - Most questions are answered here
2. **Check examples** - See working code in examples/
3. **Run the demo** - ./demo.sh shows everything working
4. **Search issues** - Someone may have had the same problem
5. **Ask questions** - Create an issue or start a discussion

## 📝 Document Conventions

- ✅ = Completed/Working
- 🔄 = In Progress
- ❌ = Not Started/Not Working
- 🟡 = Partial/Needs Work
- 📋 = Task/TODO item
- 🎯 = Goal/Target
- ⚠️ = Warning/Important
- 💡 = Tip/Note

## 🎉 You're Ready!

You now have a complete map of the documentation. Choose your path:

- **Quick start**: [QUICKSTART.md](QUICKSTART.md)
- **Deep dive**: [PROJECT_SUMMARY.md](PROJECT_SUMMARY.md)
- **Contribute**: [CONTRIBUTING.md](CONTRIBUTING.md)
- **Integrate**: [tauri/tauri-integration.md](tauri/tauri-integration.md)

Happy coding! 🚀

---

**Last Updated**: January 27, 2026  
**Documentation Version**: 1.0.0  
**Project Status**: Production-Ready (Extraction & Listing)
