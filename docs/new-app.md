# External applications documentation

Before everything else, you need to [install the SDK](/docs/install-sdk.md).

## Creating a new project

To create a new project, you just need to:

1. Create a new directory with the name of your project. You can copy the Example project to get started. If you want to make a C++ project, you can use the C++ template (Example-Cpp).
2. Add your project to the `apps/apps.js` file.
3. To flash your project, you just need to run the `make app-name_flash` command.
4. It's done! :tada:

## Editing a project

To edit a project, you just need to open your project directory and edit the files.
To flash your project, you just need to run the `make app-name_flash` command in the root directory (not in your app directory).

If you want to see the external applications documentation, you have the `api/extapi.h` file, which contains the list of all the external applications commands and their description.

# Testing your app

To test your app on device, you just need to flash it using the `make app-name_flash` command and then run it through home menu.

To test your app on the simulator, it's a little bit more complicated :

1. Clone the main Upsilon repository using `git clone --recurse-submodules https://github.com/UpsilonNumworks/Upsilon.git`
2. Copy the content of your app directory to the `Upsilon/apps/external/app/` directory
3. Build Upsilon using `make PLATFORM=simulator` (full guide is available in the Upsilon main repository).
4. Run the simulator. The following command should work, but has been tested only on the Linux simulator:

    ```shell
    ./output/*/simulator/*/epsilon.bin
    ```

5. Run the app using simulator's home menu.

## Publishing your app

To publish your app, please follow the steps below:

1. First, you need to fork the [Github repository](https://github.com/UpsilonNumworks/Upsilon-External).
2. After that, you can create a new branch and push it to your fork.
3. Then, create a pull request.
4. When the pull request is merged, the app will be published.
5. Done :tada:

If you have any questions, please join the [Omega Community Discord server](https://discord.gg/hnEqPzAJzn).
