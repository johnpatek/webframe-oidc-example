import React, { useEffect, useState } from "react";
import { UserManager } from "oidc-client-ts";

const OIDC_ENABLED = !!import.meta.env.VITE_OIDC_CLIENT_ID;

// ---- OIDC CONFIG ----
const userManager = OIDC_ENABLED
  ? new UserManager({
      authority: "https://oauth.wiremockapi.cloud",
      client_id: "my-fake-client-id",
      redirect_uri: window.location.origin, // same page
      post_logout_redirect_uri: window.location.origin,
      response_type: "code",
      scope: "openid profile email",
    })
  : null;

export default function App() {
  const [user, setUser] = useState(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState("");

  useEffect(() => {
    async function init() {
      if (!OIDC_ENABLED) {
        console.warn("OIDC client ID not set, authentication is disabled.");
        setLoading(false);
        return;
      }

      try {
        const url = window.location.href;

        // ---- CALLBACK DETECTION ----
        const isCallback =
          url.includes("code=") && url.includes("state=");

        if (isCallback) {
          const signedInUser = await userManager.signinRedirectCallback();

          // Clean URL so refresh doesn't repeat callback
          window.history.replaceState(
            {},
            document.title,
            window.location.pathname
          );

          setUser(signedInUser);
        } else {
          // Try to restore existing session
          const user = await userManager.getUser();
          setUser(user);
        }
      } catch (err) {
        setError(err instanceof Error ? err.message : String(err));
        console.error("Authentication error:", err);
      } finally {
        setLoading(false);
      }
    }

    init();
  }, []);

  async function handleLogin() {
    setLoading(true);
    setError("");

    try {
      await userManager.signinRedirect();
    } catch (err) {
      setError(err instanceof Error ? err.message : String(err));
      console.error("Login error:", err);
    }
  }

  async function handleLogout() {
    setLoading(true);
    setError("");

    try {
      await userManager.removeUser();
      setUser(null);
    } catch (err) {
      setError(err instanceof Error ? err.message : String(err));
      console.error("Logout error:", err);
    } finally {
      setLoading(false);
    }
  }

  // ---- UI STATES ----
  if (loading) {
    return <div>Loading...</div>;
  }

  if (!OIDC_ENABLED) {
    return (
      <div style={{ padding: 20 }}>
        <h1>OIDC Not Configured</h1>
        <p>
          Please set <code>VITE_OIDC_CLIENT_ID</code> in your environment to
          enable authentication.
        </p>
      </div>
    );
  }

  if (!user) {
    return (
      <div style={{ padding: 20 }}>
        <h1>Not logged in</h1>
        {error ? <pre>{error}</pre> : null}
        <button onClick={handleLogin}>
          Login
        </button>
      </div>
    );
  }

  return (
    <div style={{ padding: 20 }}>
      <h1>Welcome {user.profile?.name || "User"}</h1>

      <h3>ID Token Claims:</h3>
      <pre>{JSON.stringify(user.profile, null, 2)}</pre>

      {error ? <pre>{error}</pre> : null}
      <button onClick={handleLogout}>
        Logout
      </button>
    </div>
  );
}
