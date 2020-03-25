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
    int clientIdSave, clientIdIncoming;
    Vector3 otherPos = new Vector3(0f, 0f, 0f);
    Quaternion otherRot = new Quaternion(0f, 0f, 0f, 0f);
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

    public delegate bool AttemptConnectDelegate(ref int clientIdSave);
    public AttemptConnectDelegate AttemptConnect;

    public delegate void FreeTheConsoleDelegate();
    public FreeTheConsoleDelegate FreeTheConsole;

    public delegate IntPtr OutputConsoleMessageDelegate(string msg);
    public OutputConsoleMessageDelegate OutputConsoleMessage;

    //Game Commands
    public delegate IntPtr SendTransformDelegate(Vector3 position, Quaternion rotation);
    public SendTransformDelegate SendTransform;

    public delegate bool ReadTransformDelegate(ref Vector3 position, ref Quaternion rotation, ref int clientID);
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
            if (AttemptConnect(ref clientIdSave))
            {
                Debug.Log("Connected to Server! Client: " + clientIdSave);
                for (int i = 0; i < clientIdSave; i++)
                {
                    Instantiate(Resources.Load("Pawn"), new Vector3(0f, 1f, 0f), Quaternion.identity, gameObject.transform);
                }
                Instantiate(Resources.Load("Player"), new Vector3(0f, 0f, 0f), Quaternion.identity, gameObject.transform);
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
            SendTransform(transform.GetChild(0).localPosition, transform.GetChild(0).localRotation);
            if (ReadTransform(ref otherPos, ref otherRot, ref clientIdIncoming))
            {
                Debug.Log("Incoming ID: " + clientIdIncoming);
                transform.GetChild(clientIdIncoming).transform.localPosition = otherPos;
                transform.GetChild(clientIdIncoming).transform.localRotation = otherRot;
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
