## Contributing to the Min API and DevKit

The Min API and DevKit (Min) is accepting contributions that improve or refine any of the types, functions, content, or documentation in this library. 

Changes should be submitted as Github Pull Request.


## Housekeeping
Your pull request should: 

* Include a description of what your change intends to do
* The final commit of the pull request must pass all tests on all platforms or it will not be considered. It is desirable, but not necessary, for all the tests to pass at each commit. Please see [ReadMe.md](./ReadMe.md) for instructions to build and run the test suite.
* All commits must have clear and unambiguous commit messages, linked to Github issue tracker number (for example, the commit message includes the text "see #1974" to link it to issue number 1974.) 
* Include appropriate tests 
    * Tests should include reasonable permutations of the target fix/change
    * Include baseline changes with your change
    * All changed code should strive to have 100% code coverage


## Workflow

The Min codebase is structured in such a way that it is easy to use but contributing changes to it may leave the realm of the familiar if you don't typically work with Git submodules.

This documentation will use the command line.  It should be possible to perform these tasks with a GUI application as well.


### Getting the latest API

Refer to the [How To Updated the Min API](HowTo-UpdateTheAPI.md) documentation.

### Committing Changes in the Max API

Commiting changes in the top-level Min-DevKit is straigtforward.  If your changes are in the Min-API submodule then a little extra care will need to be taken to avoid getting tripped-up.

#### General Git Trip-Ups

If you have local changes to files inside of `source/min-api` then you will be able to build and test them as you expect.  One those changes are committed, however, the CMake scripts that automatically set the submodule to the specified commit will kick-in and your submodule will set to an older commit.  The instructions above on "Getting the latest API" may be helpful to get you back to where you want to be.

Also, be aware that your submodule (`source/min-api`) may often be set to a commit and not to the HEAD of a branch such as master.  If you don't double check (with `git branch` or `git status`) that you are on a branch it is easy to start making commits to a detached-head.

If you do make commits to a detached-head, make note of the commits (e.g. use `git log`) and then you can cherry-pick them to the branch where you want them.

Finally, it is **very important** that you push commits to the **Min-API** (the submodule) prior to pushing them to **Min-DevKit** (the host/container).  If you push your commits to Github in the opposite order then you essentially push a dangling reference to a non-existant commit when you push the Min-DevKit.

