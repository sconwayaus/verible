import * as vscode from 'vscode';
import * as vscodelc from 'vscode-languageclient/node';
import { checkAndDownloadBinaries } from './download-ls';
import { posix } from 'path';

// Global object to dispose of previous language clients.
let client: undefined | vscodelc.LanguageClient = undefined;

async function initLanguageClient() {
    const output = vscode.window.createOutputChannel('Verible Language Server');
    const config = vscode.workspace.getConfiguration('verible');
    const binary_path: string = await checkAndDownloadBinaries(config.get('languageServer.path') as string, output);

    output.appendLine(`Using executable from path: ${binary_path}`);

    const verible_ls: vscodelc.Executable = {
        command: binary_path,
        args: await config.get<string[]>('languageServer.arguments')
    };

    const serverOptions: vscodelc.ServerOptions = verible_ls;

    // Options to control the language client
    const clientOptions: vscodelc.LanguageClientOptions = {
        // Register the server for (System)Verilog documents
        documentSelector: [{ scheme: 'file', language: 'systemverilog' },
        { scheme: 'file', language: 'verilog' }],
        outputChannel: output
    };

    // Create the language client and start the client.
    output.appendLine("Starting Language Server");
    client = new vscodelc.LanguageClient(
        'verible',
        'Verible Language Server',
        serverOptions,
        clientOptions
    );
    client.start();
}

/**
 * Creates verible.filelist in the root of the workspace
 * 
 * @returns undefined
 */
async function createProjectFileList() {
    // If there is no workspace folder open then return
    if (vscode.workspace.workspaceFolders === undefined) {
        return
    }

    const config = vscode.workspace.getConfiguration('verible');
    const includeGlobPattern = await config.get('projectFileList.includeGlobPattern') as string;
    const excludeGlobPattern = await config.get('projectFileList.excludeGlobPattern') as string;
    const filelist = await vscode.workspace.findFiles(includeGlobPattern, excludeGlobPattern);

    const folderUri = vscode.workspace.workspaceFolders[0].uri;
    const fileUri = folderUri.with({ path: posix.join(folderUri.path, 'verible.filelist') });

    var writeDataStr = "";
    filelist.forEach(f => {
        writeDataStr += posix.relative(folderUri.fsPath, f.fsPath);
        writeDataStr += "\n";
    });

    const writeData = Buffer.from(writeDataStr, 'utf8');
    vscode.workspace.fs.writeFile(fileUri, writeData);
}

function addCommands(context: vscode.ExtensionContext) {
    context.subscriptions.push(
        vscode.commands.registerCommand('verible.create_project_file_list', createProjectFileList));
}

// VSCode entrypoint to bootstrap an extension
export function activate(context: vscode.ExtensionContext) {
    // If a configuration change even it fired, let's dispose
    // of the previous client and create a new one.
    vscode.workspace.onDidChangeConfiguration(async (event) => {
        if (!event.affectsConfiguration('verible')) {
            return;
        }
        if (!client) {
            return initLanguageClient();
        }
        client.stop().finally(() => {
            initLanguageClient();
        });
    });

    // Add Commands
    addCommands(context);

    return initLanguageClient();
}

// Entrypoint to tear it down.
export function deactivate(): Thenable<void> | undefined {
    if (!client) {
        return undefined;
    }
    return client.stop();
}
