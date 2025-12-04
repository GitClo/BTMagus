#include <thread>
#include "flipper_scan.h"
#include <cli/cli.h>
#include <cli/clifilesession.h>

int main() {
    auto rootMenu = std::make_unique< cli::Menu >( "BTMagus" );

    rootMenu -> Insert(
            "scan",
            [](std::ostream& out){ out << "" << "\n"; std::thread ThreadFlipperScan(scanStart); ThreadFlipperScan.join();},
            "Scan for nearby flippers " );

    cli::Cli cli( std::move(rootMenu) );
    // global exit action
    cli.ExitAction( [](auto& out){ out << "Bye.\n"; } );

    cli::CliFileSession input(cli);
    input.Start();
    return 0;
}