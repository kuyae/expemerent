﻿using System;
using System.Diagnostics;
using Expemerent.UI.Behaviors;
using Expemerent.UI.Controls;
using Expemerent.UI.Dom;
using Control = System.Windows.Forms.Control;
using Form = System.Windows.Forms.Form;

namespace Expemerent.UI
{
    /// <summary>
    /// Default implementation of <see cref="Form"/> with sciter content. 
    /// </summary>
    /// <remarks>
    /// <see cref="SciterForm"/> and <see cref="SciterControl"/> classes provide an extended services working with Htmlayout/Sciter.    
    /// [*] SciterControls management
    /// [*] Behaviors management
    /// </remarks>
    public class SciterForm : Form
    {
        #region Private data
        /// <summary>
        /// See <see cref="Host"/> property
        /// </summary>
        private ControlsHost _host;

        /// <summary>
        /// Gets the <see cref="ISciterHost"/> instance 
        /// </summary>
        private ControlsHost Host
        {
            [DebuggerStepThrough]
            get { return _host ?? (_host = new ControlsHost(this) { AutoValidate = AutoValidate }); }
        } 
        #endregion

        #region Construction
        /// <summary>
        /// Creates a new instance of the <see cref="SciterForm"/> class
        /// </summary>
        public SciterForm()
        {
        } 
        #endregion

        #region Public interface
        /// <summary>
        /// Occurs when control needs a behavior instance
        /// </summary>
        public event EventHandler<AttachBehaviorEventArgs> AttachBehavior
        {
            add { Host.AttachBehavior += value; }
            remove { Host.AttachBehavior -= value; ; }
        }

        /// <summary>
        /// Handles scripting method
        /// </summary>
        public event EventHandler<ScriptingMethodCallEventArgs> ScriptingMethodCall
        {
            add { Host.ScriptingMethodCall += value; }
            remove { Host.ScriptingMethodCall -= value; }
        }

        /// <summary>
        /// Occurs when control needs a behavior instance
        /// </summary>
        public event EventHandler<DocumentCompleteEventArgs> DocumentComplete
        {
            add { Host.DocumentComplete += value; }
            remove { Host.DocumentComplete -= value; }
        }

        /// <summary>
        /// Occurs when control needs a behavior instance
        /// </summary>
        public event EventHandler<CallbackHostEventArgs> CallbackHost
        {
            add { Host.CallbackHost += value; }
            remove { Host.CallbackHost -= value; }
        }

        /// <summary>
        /// Gets a collection of sciter controls
        /// </summary>
        public ControlsCollection SciterControls
        {
            [DebuggerStepThrough]
            get { return Host.SciterControls; }
        }

        /// <summary>
        /// Gets root dom element of the currently loaded document
        /// </summary>
        public Element RootElement
        {
            [DebuggerStepThrough]
            get { return Host.RootElement; }
        }

        /// <summary>
        /// Loads Html from the resource. The html loading can be delayed if window handle was not created.
        /// </summary>
        /// <typeparam name="TResourceBase">Used as base namespace to find a managed resource</typeparam>
        /// <param name="resourceName">Relative, case sensitive, resource name</param>
        public void LoadHtmlResource<TResourceBase>(string resourceName)
        {
            Host.LoadResource<TResourceBase>(resourceName);
        }

        /// <summary>
        /// Loads Html from the resource. The html loading can be delayed if window handle was not created.
        /// </summary>
        /// <param name="resourceName">Case sensitive, resource name</param>
        public void LoadHtmlResource(String resourceName)
        {
            Host.LoadResource(resourceName);
        }

        /// <summary>
        /// Loads Html from the resource. The html loading can be delayed if window handle was not created.
        /// </summary>
        /// <typeparam name="baseUri">Used as a base URI in the data requests</typeparam>
        /// <param name="html">Html text</param>
        public void LoadHtml(string baseUri, string html)
        {
            Host.LoadHtml(baseUri, html);
        }

        /// <summary>
        /// Reloads previously loaded document
        /// </summary>
        public void Reload()
        {
            Host.Reload();
        }

        /// <summary>
        /// Causes validation in all sciter controls
        /// </summary>
        public bool PerformValidation()
        {
            return Host.PerformValidation();
        }

        /// <summary>
        /// Calls scripting method
        /// </summary>
        public object Call(string functionName, params object[] args)
        {
            return Host.Call(functionName, args);
        }

        /// <summary>
        /// Registers scripting class 
        /// </summary>
        public void RegisterClass<TType>() where TType : new()
        {
            Host.RegisterClass<TType>();
        }
        #endregion

        #region Event handling
        /// <summary>
        /// Handles <see cref="HandleCreated"/> event
        /// </summary>
        protected override void OnHandleCreated(EventArgs e)
        {
            if (Site == null || !Site.DesignMode)
                Host.AttachToControl();

            base.OnHandleCreated(e);
        }

        /// <summary>
        /// Handles <see cref="HandleDestroyed"/> event
        /// </summary>
        protected override void OnHandleDestroyed(EventArgs e)
        {
            base.OnHandleDestroyed(e);

            if (Site == null || !Site.DesignMode)
                Host.DetachFromControl();
        }
        #endregion

        #region Internal implementation
        /// <summary>
        /// Handles <see cref="AutoValidate"/> property changes
        /// </summary>
        protected override void OnAutoValidateChanged(EventArgs e)
        {
            base.OnAutoValidateChanged(e);
            Host.AutoValidate = AutoValidate;
        } 
        #endregion
    }
}
