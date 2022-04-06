# thprac
A tool for practicing Touhou Shmups

I will indefinitely cease all future development on thprac/Marketeer, you are free to continue working on it, granting that you follow the license's terms.
Due to my insufficient skill, the code is glutted with obscure writings and terrible logic, turning the whole thing into a complete mess. I'm sorry if that disturbs you.

# How to build:
thprac: This is a self-complete package that includes extra third-party libraries. Just build the solution and you're done.
marketeer-server: You will need two extra dependencies: nng and wolfssl. On Windows, you can use vcpkg. On Linux, you can simply build from the source code. It also lacks any kind of makefile so you need to make your own.