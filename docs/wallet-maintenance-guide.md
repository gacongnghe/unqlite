# Gnosis Safe Wallet Maintenance Mode

## Overview

This guide explains how to run Gnosis Safe in **wallet maintenance mode** - which allows you to use existing Safes for transactions but removes the ability to create new Safes or perform certain administrative functions.

## What Gets Disabled in Wallet-Only Mode

### ❌ Components Removed:
- **Safe Transaction Service** - Not needed for simple wallet operations
- **Safe Config Service** - Not needed for wallet maintenance
- **Local PostgreSQL** - Can connect to external services instead
- **Safe Creation Features** - No new Safe deployment capability
- **Admin Interfaces** - No transaction service admin panel available

### ✅ Components Kept:
- **Safe React Frontend** - Essential for wallet interface
- **Safe Client Gateway** - Required for frontend-backend communication
- **PostgreSQL** - Minimal database for session management
- **Redis** - Caching layer for performance
- **Nginx** - Reverse proxy and access point

## Quick Start for Wallet Maintenance

### 1. Start Wallet-Only Mode
```bash
# Start wallet maintenance mode
make wallet-only
```

### 2. Access Your Wallet
- Open browser to http://localhost
- Connect your wallet
- Access existing Safes

### 3. Stop Services
```bash
make wallet-stop
```

## Configuration Options

### External Service Integration

You can configure the wallet-only mode to use external Safe services instead of running all components locally:

```env
# In your .env file - point to external services
REACT_APP_GATEWAY_URL=https://safe-client.gnosis.io
REACT_APP_TRANSACTION_SERVICE_URL=https://safe-transaction.gnosis.io
REACT_APP_CONFIG_SERVICE_URL=https://safe-config.gnosis.io
```

### Benefits of External Services
- **Reduced Resource Usage** - No Ethereum node indexing required
- **Automatic Updates** - Always using the latest service features
- **Simplified Maintenance** - Less components to maintain
- **Better Reliability** - Official Gnosis services are highly available

## What You Can Do in Wallet-Only Mode

### ✅ Wallet Operations
- **Connect Wallets** - MetaMask, WalletConnect, etc.
- **Access Existing Safes** - View balance and transaction history
- **Submit Transactions** - Create, sign, and execute transactions
- **Manage Owners** - Add/remove owners (if you have permission)
- **Threshold Changes** - Modify confirmation requirements
- **Module Management** - Add/remove modules and guards
- **Fallback Handler** - Configure fallback handlers

### ✅ Safe Features
- **Multi-signature Transactions** - Submit for approval/sign existing
- **Batch Transactions** - Create transaction bundles
- **Safe Apps** - Use integrations (1inch, Compound, etc.)
- **Settings Configuration** - Safe name, logo, etc.
- **Safe Tokens** - View token balances and NFTs

## What You Cannot Do

### ❌ Administrative Operations
- **Create New Safes** - No Safe deployment capability
- **Service Administration** - No transaction service admin panel
- **Configuration Management** - Cannot modify global settings
- **Transaction Indexing** - Won't index blockchain history locally

### ❌ Development Features
- **Contract Deployment** - Cannot deploy new Safe contracts
- **Service Monitoring** - Limited backend service control
- **Advanced Logging** - Reduced transaction service logging

## Resource Requirements

### Minimal Configuration
- **CPU**: 1-2 cores
- **RAM**: 2-4 GB
- **Storage**: 5-10 GB
- **Network**: Basic bandwidth (much less than full deployment)

### Compared to Full Setup
- **~70% less resources** required
- **~80% faster startup time**
- **No Ethereum node required**
- **Simple backup/restore**

## Security Considerations

### Reduced Attack Surface
- **Fewer Running Services** - Less components vulnerable to attacks
- **External Validation** - Rely on official Gnosis service security
- **Simplified Access** - No admin interfaces to secure

### Network Requirements
- **External API Access** - Requires internet connection to Gnosis services
- **Dependency on External Services** - If Gnosis services are down, wallet is affected
- **Data Privacy** - Your Safe usage data goes through Gnosis services

## Maintenance Commands

```bash
# Check wallet-only services
docker-compose -f docker-compose.wallet-only.yml ps

# View logs
docker-compose -f docker-compose.wallet-only.yml logs -f

# Update wallet-only container images
docker-compose -f docker-compose.wallet-only.yml pull
docker-compose -f docker-compose.wallet-only.yml up -d

# Health check
curl http://localhost/health
```

## Switching Between Modes

### From Full to Wallet-Only
```bash
# Stop full services
make stop

# Start wallet-only
make wallet-only
```

### From Wallet-Only to Full
```bash
# Stop wallet-only
make wallet-stop

# Start full services
make start
```

## Troubleshooting

### Common Issues

1. **Frontend not loading**:
   ```bash
   # Check if client-gateway is running
   docker-compose -f docker-compose.wallet-only.yml ps
   ```

2. **Cannot connect to Safe**:
   - Verify internet connection for external services
   - Check your .env configuration for external service URLs

3. **Performance issues**:
   - Ensure you have adequate resources allocated
   - Check Redis connectivity for caching

## Best Practices

### For Production
- Use external Gnosis services for better reliability
- Set up SSL certificates for HTTPS access
- Configure proper firewall rules
- Regular monitoring of wallet access

### For Development
- Keep both configurations available
- Use wallet-only for testing existing integrations
- Use full setup for developing new Safe features

---

**Note**: Wallet-only mode is perfect when you have existing Safes and primarily need wallet functionality without the overhead of running all backend services locally.