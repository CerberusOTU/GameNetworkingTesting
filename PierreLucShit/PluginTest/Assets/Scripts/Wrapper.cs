using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

public struct CS_to_Plugin_Functions
{
    public IntPtr MultiplyVectors;
    public IntPtr MultiplyInts;
    public IntPtr RandomFloat;
    public IntPtr StartClient;

    public bool Init(IntPtr pluginHandle)
    {
        MultiplyVectors = Marshal.GetFunctionPointerForDelegate(new Func<Vector3, Vector3, Vector3>(Wrapper.multiplyVectors));
        MultiplyInts = Marshal.GetFunctionPointerForDelegate(new Func<int, int, int>(Wrapper.MultiplyInts));
        RandomFloat = Marshal.GetFunctionPointerForDelegate(new Func<float>(Wrapper.GetFloat));

        return true;
    }
}

public class Wrapper : MonoBehaviour
{
    int clientIdSave;
    Vector3 otherPos = new Vector3(0f, 0f, 0f);
    bool connected = false;


    [Header("Server")]
    [SerializeField] private string serverAddress = "127.0.0.1";

    private const string path = "/Plugins/NetworkingPlugin.dll";
    private IntPtr Plugin_Handle;
    private CS_to_Plugin_Functions Plugin_Functions = new CS_to_Plugin_Functions();

    public delegate void InitPluginDelegate(CS_to_Plugin_Functions functions);
    public InitPluginDelegate InitPlugin;

    public delegate void InitConsoleDelegate();
    public InitConsoleDelegate InitConsole;

    public delegate bool InitClientDelegate(string server);
    public InitClientDelegate InitClient;

    public delegate bool AttemptConnectDelegate();
    public AttemptConnectDelegate AttemptConnect;

    public delegate void FreeTheConsoleDelegate();
    public FreeTheConsoleDelegate FreeTheConsole;

    public delegate IntPtr OutputConsoleMessageDelegate(string msg);
    public OutputConsoleMessageDelegate OutputConsoleMessage;

    //Game Commands
    public delegate IntPtr SendTransformDelegate(Vector3 transform);
    public SendTransformDelegate SendTransform;

    public delegate void ReadTransformDelegate(ref Vector3 transform, ref int clientID);
    public ReadTransformDelegate ReadTransform;


    private void InitPluginFunctions()
    {
        InitPlugin = ManualPluginImporter.GetDelegate<InitPluginDelegate>(Plugin_Handle, "InitPlugin");
        InitConsole = ManualPluginImporter.GetDelegate<InitConsoleDelegate>(Plugin_Handle, "InitConsole");
        InitClient = ManualPluginImporter.GetDelegate<InitClientDelegate>(Plugin_Handle, "InitClient");
        AttemptConnect = ManualPluginImporter.GetDelegate<AttemptConnectDelegate>(Plugin_Handle, "AttemptConnect");
        FreeTheConsole = ManualPluginImporter.GetDelegate<FreeTheConsoleDelegate>(Plugin_Handle, "FreeTheConsole");
        OutputConsoleMessage = ManualPluginImporter.GetDelegate<OutputConsoleMessageDelegate>(Plugin_Handle, "OutputMessageToConsole");

        //Game Commands
        SendTransform = ManualPluginImporter.GetDelegate<SendTransformDelegate>(Plugin_Handle, "SendTransform");
        ReadTransform = ManualPluginImporter.GetDelegate<ReadTransformDelegate>(Plugin_Handle, "ReadTransform");




    }


    private void Awake()
    {
        Plugin_Handle = ManualPluginImporter.OpenLibrary(Application.dataPath + path);
        Plugin_Functions.Init(Plugin_Handle);

        InitPluginFunctions();

        InitPlugin(Plugin_Functions);
        InitConsole();
        InitClient(serverAddress);
    }

    void Update()
    {
        if (Input.GetKeyDown(KeyCode.C) && !connected)
        {
            if (AttemptConnect())
            {
                Debug.Log("Connected to Server!");
                connected = true;
            }
            else
            {
                Debug.Log("Error: Could not connect to server.");
                connected = false;
            }
        }

        if (connected)
        {
            if (Input.GetKeyDown(KeyCode.M))
            {
                SendTransform(transform.GetChild(0).transform.position);
            }
            if (Input.GetKeyDown(KeyCode.N))
            {
                ReadTransform(ref otherPos, ref clientIdSave);
                transform.GetChild(clientIdSave).transform.position = otherPos;
            }
        }
    }

    private void OnApplicationQuit()
    {
        FreeTheConsole();
        ManualPluginImporter.CloseLibrary(Plugin_Handle);
    }

    public static Vector3 multiplyVectors(Vector3 v1, Vector3 v2)
    {
        return new Vector3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
    }

    public static int MultiplyInts(int i1, int i2)
    {
        return i1 * i2;
    }

    public static float GetFloat()
    {
        return UnityEngine.Random.Range(0.0f, 100.0f);
    }
}
