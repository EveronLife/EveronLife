# Contribution Guidelines

Welcome and thanks for your interest in contributing to EveronLife.
This is a set of guidelines to help us and you contribute to the project.
Contributions can occur in many different ways.

## Development
We are using git for version control and GitHub as platform to host it. If you are new to contributing to GitHub repositories consider checking out this [guide](https://github.com/firstcontributions/first-contributions) that outlines the preparations to start proposing changes.

### Communication
The primary platform for development and its communication is this repository. Any important information, findings, concept designs, etc must at least be shared once in the related discussions/issues, even if you were using a voice chat to collaborate on its creation. This way everyone can read up on the thought process. Also, decisions and lessons learned are not forgotten in the future. Please try to write your comments as compact as possible to avoid short live chat-like comment chains.

### Contribution process
1. `Propose your idea` Check if nobody has suggested your idea before by searching in the [discussions](https://github.com/EveronLife/EveronLife/discussions) and [issues](https://github.com/EveronLife/EveronLife/issues). If it is not worked on and has not been declined previously make a [new discussion post](https://github.com/EveronLife/EveronLife/discussions/new?category=ideas) about your idea.
2. `Brainstorming` Depending on the category of the idea, everyone gets to share their suggestions both in terms of gameplay as well as technical aspects.
3. `Technical scouting` Once the goals are defined it is time to sort out implementation details and scout for existing functionality to utilize. Document your findings in the discussion. Even if they might not end up being useful directly, other contributors might see something they need for a task they work on!
4. `Implementation concepts` Once all available possibilities are collected, one or more implementation concepts are to be created. Those outline what aspects are going to be added in what way, where, and how they interact with other components from the framework or the vanilla game. These concepts are the basis for kicking off the development. If multiple concepts are outlined, a proof of concept for each of them should be presented to decide which one to fully implement. You can let people vote using 👍 and 👎 emojis and use the `voting` tag to let others know you are looking for opinions on which one to choose. If new concepts are suggested during voting you might want to repeat this step. 
6. `Development kick-off` Once a concept is selected the discussion is converted into an issue and the concept can be fully implemented. Someone will usually volunteer to take responsibility for the issue and becomes the person to interact with about it. The maintainers interact with the contributors assigned to the task, to check on the progress and schedule it for a milestone. If additional contributors are needed, they can be requested using the `<skill> wanted` tags.
7. `Testing` During development you want to regularly test your current changes. Avoid committing code that does not compile, as that keeps others from working on sub-tasks of the issue.
8. `Review` When the development concludes and you have tested everything, to make sure all proposed changes work, without breaking other framework aspects, then you make a pull request and get it reviewed. More about this in the [Getting your changes merged](#changes_merged) section. Here the maintainers check for software quality or other formality issues.
9. `Code gets merged` After the review the code is merged as a single squashed commit so that it can be either selected into a release branch or reverted if something goes terribly wrong. Everyone who worked on the issue is now listed as contributor to the repository. Congratulations and thank you for your contribution ❤️
10. `Release` When the milestone the issue was scheduled for releases, the change is cherry picked into the release branch and is then publicly accessible following the framework mod update on the Workshop. *Note: There might be some release candidates and preview branches available later, this topic is yet to be further discussed.*

### Decision making
It can be difficult to decide on design and implementation concepts or other aspects of the development process. Here are some general guidelines on how to come to a conclusion:
- Every opinion should be heard. You should list arguments for and against something if you want to convince others or have them understand your point of view. A decision can only be made if all relevant information is shared. You must not repeat yourself if someone keeps ignoring explanations, but nobody should be ignored with a "that is not a good idea, but I will not explain why" kind of response.
- Objectively better solutions must be chosen, even if it does not match personal preference. 
- If two options have no real advantages over each other and it is a matter of personal preference, always follow the existing conventions for this repository (e.g. [Code Styleguide](#code_guidelines)). If there are none, see if Arma Reforger has any guidelines or what is more common in terms of gameplay design / technical aspects. You might also check for what is more common amongst the professional industry in the matter. e.g. most programmers do X, most artists prefer to do Y. Again provide arguments/sources for those if you want to convince others that this is the way *(pun intended)*. 
- If even that does not help you to decide, let people vote using 👍 and 👎 emojis and the `voting` tag to make them aware of the fact that you need help coming to a conclusion.

### Git Commit Messages
* Use the present tense ("Add feature" not "Added feature")
* Use the imperative mood ("Move cursor to..." not "Moves cursor to...")
* Limit the first line to 72 characters or less
* Reference issues and pull requests liberally after the first line
* Consider using [Conventional Commits](https://www.conventionalcommits.org/en/v1.0.0/)

### Recognising Co-Authors
It's important to recognise the contribution of everyone who works on Everon Life. As such there may be times where you commit a change that was worked on alongside others who didn't directly commit changes to source control. For example, you may be using models or textures that were made by another contributor that were checked in by you, or design documents that were never pushed to source control.

In order for this contribution to be recognized please add them as a co-author to **at least one** of your pull requests commits: [GitHub Guide](https://docs.github.com/en/enterprise-cloud@latest/pull-requests/committing-changes-to-your-project/creating-and-editing-commits/creating-a-commit-with-multiple-authors).

### <a name="code_guidelines"></a> Code Styleguide
Currently, we do not have yet automatic tooling to enforce our code style guide. For now please follow the guidelines published on the Community Wiki.
* [Do's and Don'ts](https://community.bistudio.com/wiki/Arma_Reforger:Scripting:_Do%27s_and_Don%27ts)
* [Conventions](https://community.bistudio.com/wiki/Arma_Reforger:Scripting:_Conventions)
* [Performance](https://community.bistudio.com/wiki/Arma_Reforger:Scripting:_Performance)
* [Values](https://community.bistudio.com/wiki/Arma_Reforger:Scripting:_Values)

### Directory guidelines
* [Arma Reforger directory structure](https://community.bistudio.com/wiki/Arma_Reforger:Directory_Structure)

### <a name="changes_merged"></a> Getting your changes merged
- Any changes you want to propose are to be developed on a fork of the repository you create. There you can collaborate with others and have them push to your fork. 
- You might want to create a branch on your fork when working on multiple issues at the same time. It is suggested that you prefix them according to the category e.g. `bug/atm_system_compile_fix` or `feature/vehicle_shop` or `enhancement/apple_model_size_reduction`.
- Once your work is done and testing showed no problems, you open a pull request for the issue you worked on.
- On the **first** pull request you file, you will be asked automatically to sign a [contributor license agreement (CLA)](https://en.wikipedia.org/wiki/Contributor_License_Agreement) via a bot comment. Please click on the badge inside it and go through the quick process of digitally signing it. Your pull request will not be accepted until all contributors on it have signed the CLA.
This CLA provides everyone involved with a legal basis, to be sure that intellectual property rights are being respected. Nobody wants stolen models, scripts, or otherwise non-credited or illegal assets in our framework.

## Release process
The release process will follow the [Semantic Versioning](https://semver.org/) standard. We will create release branches for each major version and each minor version. Our release process will be roughly modeled after what is written [here](https://trunkbaseddevelopment.com/branch-for-release/)
