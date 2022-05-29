# Contribution Guidelines

Welcome and thanks for your interest in contributing to EveronLife.
This is a set of guidelines to help us and you contribute to the project.
Contributions can occur in many different ways.

## Development process
We are using git for version control and GitHub as platform to host it. If you are new to contributing to GitHub repositories consider checking out this [guide](https://github.com/firstcontributions/first-contributions) that outlines the perperations to start proposing changes.

### Communication
The primary platform for development and its communication is this repository. Any important information, findings, concept designs, etc must at least be shared once in the related discussions/issues, even if you were using a voice chat to collaborate on its creation. This way everyone can read up on the thought process. Also, decisions and lessons learned are not forgotten in the future. Please try to write your comments as compact as possible to avoid short live chat-like comment chains.

### Git Commit Messages
* Use the present tense ("Add feature" not "Added feature")
* Use the imperative mood ("Move cursor to..." not "Moves cursor to...")
* Limit the first line to 72 characters or less
* Reference issues and pull requests liberally after the first line
* Consider using [Conventional Commits](https://www.conventionalcommits.org/en/v1.0.0/)

### Code Styleguide
Currently, we do not have yet automatic tooling to enforce our code style guide. For now please follow the guidelines published on the Community Wiki.
* [Do's and Don'ts](https://community.bistudio.com/wiki/Arma_Reforger:Scripting:_Do%27s_and_Don%27ts)
* [Conventions](https://community.bistudio.com/wiki/Arma_Reforger:Scripting:_Conventions)
* [Performance](https://community.bistudio.com/wiki/Arma_Reforger:Scripting:_Performance)
* [Values](https://community.bistudio.com/wiki/Arma_Reforger:Scripting:_Values)

### Getting your changes merged
- Any changes you want to propose are to be developed on a fork of the repository you create. There you can collaborate with others and have them push to your fork. 
- You might want to create a branch on your fork when working on multiple issues at the same time. It is suggested that you prefix them according to the category e.g. `bug/atm_system_compile_fix` or `feature/vehicle_shop` or `enhancement/apple_model_size_reduction`.
- Once your work is done and testing showed no problems, you open a pull request for the issue you worked on.
- On the **first** pull request you file, you will be asked automatically to sign a [contributor license agreement (CLA)](https://en.wikipedia.org/wiki/Contributor_License_Agreement) via a bot comment. Please click on the badge inside it and go through the quick process of digitally signing it. Your pull request will not be accepted until all contributors on it have signed the CLA.
This CLA provides everyone involved with a legal basis, to be sure that intellectual property rights are being respected. Nobody wants stolen models, scripts, or otherwise non-credited or illegal assets in our framework.

## Release process
The release process will follow the [Semantic Versioning](https://semver.org/) standard. We will create release branches for each major version and each minor version. Our release process will be roughly modeled after what is written [here](https://trunkbaseddevelopment.com/branch-for-release/)
